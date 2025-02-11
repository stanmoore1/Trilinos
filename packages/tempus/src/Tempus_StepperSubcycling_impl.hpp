// @HEADER
// ****************************************************************************
//                Tempus: Copyright (2017) Sandia Corporation
//
// Distributed under BSD 3-clause license (See accompanying file Copyright.txt)
// ****************************************************************************
// @HEADER

#ifndef Tempus_StepperSubcycling_impl_hpp
#define Tempus_StepperSubcycling_impl_hpp

#include "Tempus_StepperFactory.hpp"
#include "Tempus_TimeStepControlStrategyConstant.hpp"
#include "Tempus_TimeStepControlStrategyBasicVS.hpp"
#include "Tempus_IntegratorObserverSubcycling.hpp"
#include "Tempus_IntegratorObserverNoOp.hpp"

#include "Teuchos_VerboseObjectParameterListHelpers.hpp"
#include "Thyra_VectorStdOps.hpp"


namespace Tempus {


template<class Scalar>
StepperSubcycling<Scalar>::StepperSubcycling()
{
  using Teuchos::RCP;
  using Teuchos::ParameterList;

  this->setStepperType(        "Subcycling");
  this->setUseFSAL(            this->getUseFSALDefault());
  this->setICConsistency(      this->getICConsistencyDefault());
  this->setICConsistencyCheck( this->getICConsistencyCheckDefault());

  this->setObserver(Teuchos::rcp(new StepperSubcyclingObserver<Scalar>()));
  scIntegrator_ = Teuchos::rcp(new IntegratorBasic<Scalar>());
  scIntegrator_->setTimeStepControl();
  scIntegrator_->setObserver(
    Teuchos::rcp(new IntegratorObserverNoOp<Scalar>()));

  this->setSubcyclingPrintDtChanges(false);

  RCP<ParameterList> tempusPL = scIntegrator_->getTempusParameterList();

  { // Set default subcycling Stepper to Forward Euler.
    tempusPL->sublist("Default Integrator")
                 .set("Stepper Name", "Default Subcycling Stepper");
    RCP<ParameterList> stepperPL = Teuchos::parameterList();
    stepperPL->set("Stepper Type", "Forward Euler");
    tempusPL->set("Default Subcycling Stepper", *stepperPL);

    auto sf = Teuchos::rcp(new Tempus::StepperFactory<Scalar>());
    auto stepperFE = sf->createStepperForwardEuler(Teuchos::null,Teuchos::null);
    setSubcyclingStepper(stepperFE);
  }

  // Keep the default SolutionHistory settings:
  //  * 'Storage Type' = "Undo"
  //  * 'Storage Limit' = 2
  // Also
  //  * No checkpointing within the subcycling, but can restart from
  //    failed subcycle step.

  // Keep the default TimeStepControl settings for subcycling:
  //  * Finish exactly on the full timestep.
  //  * No solution output during the subcycling.
  //  * Variable time step size.
  // Set the default initial time step size.
  {
    tempusPL->sublist("Default Integrator")
             .sublist("Time Step Control")
                 .set("Initial Time Step", std::numeric_limits<Scalar>::max());
  }
}


template<class Scalar>
StepperSubcycling<Scalar>::StepperSubcycling(
  const Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> >& appModel,
  const Teuchos::RCP<StepperSubcyclingObserver<Scalar> >& obs,
  const Teuchos::RCP<IntegratorBasic<Scalar> >& scIntegrator,
  bool useFSAL,
  std::string ICConsistency,
  bool ICConsistencyCheck)
{
  this->setStepperType(        "Subcycling");
  this->setUseFSAL(            useFSAL);
  this->setICConsistency(      ICConsistency);
  this->setICConsistencyCheck( ICConsistencyCheck);

  this->setObserver(obs);
  scIntegrator_ = scIntegrator;
  this->setSubcyclingPrintDtChanges(false);

  if (appModel != Teuchos::null) {
    this->setModel(appModel);
    this->initialize();
  }
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingStepper(
  Teuchos::RCP<Stepper<Scalar> > stepper)
{
  scIntegrator_->setStepperWStepper(stepper);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingMinTimeStep(Scalar MinTimeStep)
{
  scIntegrator_->getNonConstTimeStepControl()->setMinTimeStep(MinTimeStep);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingInitTimeStep(Scalar InitTimeStep)
{
  scIntegrator_->getNonConstTimeStepControl()->setInitTimeStep(InitTimeStep);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingMaxTimeStep(Scalar MaxTimeStep)
{
  scIntegrator_->getNonConstTimeStepControl()->setMaxTimeStep(MaxTimeStep);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingStepType(std::string stepType)
{
  scIntegrator_->getNonConstTimeStepControl()->setStepType(stepType);

  auto tsc = scIntegrator_->getNonConstTimeStepControl();
  auto tscStrategy = tsc->getTimeStepControlStrategy();
  tscStrategy->clearObservers();

  Teuchos::RCP<TimeStepControlStrategy<Scalar> > strategy =
    Teuchos::rcp(new TimeStepControlStrategyConstant<Scalar>());
  if (stepType == "Variable")
    strategy = Teuchos::rcp(new TimeStepControlStrategyBasicVS<Scalar>());

  tscStrategy->addStrategy(strategy);

  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingMaxFailures(int MaxFailures)
{
  scIntegrator_->getNonConstTimeStepControl()->setMaxFailures(MaxFailures);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::
setSubcyclingMaxConsecFailures(int MaxConsecFailures)
{
  scIntegrator_->getNonConstTimeStepControl()->setMaxConsecFailures(MaxConsecFailures);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::
setSubcyclingScreenOutputIndexInterval(int i)
{
  scIntegrator_->setScreenOutputIndexInterval(i);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::
setSubcyclingScreenOutputIndexList(std::string s)
{
  scIntegrator_->setScreenOutputIndexList(s);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::
setSubcyclingTimeStepControlStrategy(
  Teuchos::RCP<TimeStepControlStrategy<Scalar> > tscs)
{
  scIntegrator_->getNonConstTimeStepControl()->getTimeStepControlStrategy()->clearObservers();
  scIntegrator_->getNonConstTimeStepControl()->setTimeStepControlStrategy(tscs);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingIntegratorObserver(
  Teuchos::RCP<IntegratorObserver<Scalar> > obs)
{
  scIntegrator_->setObserver(obs);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setSubcyclingPrintDtChanges(
  bool printDtChanges)
{
  scIntegrator_->getNonConstTimeStepControl()->setPrintDtChanges(printDtChanges);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setModel(
  const Teuchos::RCP<const Thyra::ModelEvaluator<Scalar> >& appModel)
{
  scIntegrator_->setStepper(appModel);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setNonConstModel(
  const Teuchos::RCP<Thyra::ModelEvaluator<Scalar> >& appModel)
{
  setModel(appModel);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setObserver(
  Teuchos::RCP<StepperObserver<Scalar> > obs)
{
  if (obs != Teuchos::null) stepperSCObserver_ =
    Teuchos::rcp_dynamic_cast<StepperSubcyclingObserver<Scalar> > (obs, true);

  if (stepperSCObserver_ == Teuchos::null)
    stepperSCObserver_ = Teuchos::rcp(new StepperSubcyclingObserver<Scalar>());

  this->isInitialized_ = false;
}


template<class Scalar>
Teuchos::RCP<StepperObserver<Scalar> >
StepperSubcycling<Scalar>::getObserver() const
{ return stepperSCObserver_; }


template<class Scalar>
void StepperSubcycling<Scalar>::initialize()
{
  Teuchos::RCP<Teuchos::FancyOStream> out = this->getOStream();
  bool isValidSetup = true;

  if ( !(this->getICConsistency() == "None" ||
         this->getICConsistency() == "Zero" ||
         this->getICConsistency() == "App"  ||
         this->getICConsistency() == "Consistent") ) {
    isValidSetup = false;
    *out << "The IC consistency does not have a valid value!\n"
         << "('None', 'Zero', 'App' or 'Consistent')\n"
         << "  ICConsistency  = " << this->getICConsistency() << "\n";
  }
  scIntegrator_->initialize();


  if (stepperSCObserver_ == Teuchos::null) {
    isValidSetup = false;
    *out << "The Subcycling observer is not set!\n";
  }

  if (isValidSetup)
    this->isInitialized_ = true;   // Only place it is set to true.
  else
    this->describe(*out, Teuchos::VERB_MEDIUM);
}


template<class Scalar>
bool StepperSubcycling<Scalar>::isExplicit() const
{ return scIntegrator_->getStepper()->isExplicit(); }


template<class Scalar>
bool StepperSubcycling<Scalar>::isImplicit() const
{ return scIntegrator_->getStepper()->isImplicit(); }


template<class Scalar>
bool StepperSubcycling<Scalar>::isExplicitImplicit() const
{ return scIntegrator_->getStepper()->isExplicitImplicit(); }


template<class Scalar>
bool StepperSubcycling<Scalar>::isOneStepMethod() const
{ return scIntegrator_->getStepper()->isOneStepMethod(); }


template<class Scalar>
bool StepperSubcycling<Scalar>::isMultiStepMethod() const
{ return scIntegrator_->getStepper()->isMultiStepMethod(); }


template<class Scalar>
Scalar StepperSubcycling<Scalar>::getOrder() const
{ return scIntegrator_->getStepper()->getOrder(); }


template<class Scalar>
Scalar StepperSubcycling<Scalar>::getOrderMin() const
{ return scIntegrator_->getStepper()->getOrderMin(); }


template<class Scalar>
Scalar StepperSubcycling<Scalar>::getOrderMax() const
{ return scIntegrator_->getStepper()->getOrderMax(); }


template<class Scalar>
OrderODE StepperSubcycling<Scalar>::getOrderODE() const
{ return scIntegrator_->getStepper()->getOrderODE(); }


template<class Scalar>
Scalar StepperSubcycling<Scalar>::getInitTimeStep(
  const Teuchos::RCP<SolutionHistory<Scalar> >& solutionHistory) const
{ return scIntegrator_->getStepper()->getInitTimeStep(solutionHistory); }


template<class Scalar>
void StepperSubcycling<Scalar>::setInitialGuess(
  Teuchos::RCP<const Thyra::VectorBase<Scalar> > initialGuess)
{
  scIntegrator_->getStepper()->setInitialGuess(initialGuess);
  this->isInitialized_ = false;
}


template<class Scalar>
void StepperSubcycling<Scalar>::setInitialConditions(
  const Teuchos::RCP<SolutionHistory<Scalar> >& solutionHistory)
{ scIntegrator_->getStepper()->setInitialConditions(solutionHistory); }


template<class Scalar>
void StepperSubcycling<Scalar>::setSolver(
  Teuchos::RCP<Thyra::NonlinearSolverBase<Scalar> > solver)
{
  scIntegrator_->getStepper()->setSolver(solver);
  this->isInitialized_ = false;
}


template<class Scalar>
Teuchos::RCP<Thyra::NonlinearSolverBase<Scalar> >
StepperSubcycling<Scalar>::getSolver() const
{ return scIntegrator_->getStepper()->getSolver(); }


template<class Scalar>
void StepperSubcycling<Scalar>::takeStep(
  const Teuchos::RCP<SolutionHistory<Scalar> >& solutionHistory)
{
  using Teuchos::RCP;

  TEMPUS_FUNC_TIME_MONITOR("Tempus::StepperSubcycling::takeStep()");
  {
    TEUCHOS_TEST_FOR_EXCEPTION(solutionHistory->getNumStates() < 2,
      std::logic_error,
      "Error - StepperSubcycling<Scalar>::takeStep(...)\n"
      "Need at least two SolutionStates for Subcycling.\n"
      "  Number of States = " << solutionHistory->getNumStates() << "\n"
      "Try setting in \"Solution History\" \"Storage Type\" = \"Undo\"\n"
      "  or \"Storage Type\" = \"Static\" and \"Storage Limit\" = \"2\"\n");

    stepperSCObserver_->observeBeginTakeStep(solutionHistory, *this);
    RCP<SolutionState<Scalar> > currentState=solutionHistory->getCurrentState();
    RCP<SolutionState<Scalar> > workingState=solutionHistory->getWorkingState();

    auto scTSC = scIntegrator_->getNonConstTimeStepControl();
    scTSC->setInitTime   (currentState->getTime());
    scTSC->setInitIndex  (0);
    scTSC->setFinalTime  (workingState->getTime());
    scTSC->setMaxTimeStep(workingState->getTimeStep());

    Teuchos::RCP<SolutionStateMetaData<Scalar> > scMD =
      rcp(new SolutionStateMetaData<Scalar>());
    scMD->copy(currentState->getMetaData());
    scMD->setDt(scTSC->getInitTimeStep());
    scMD->setOrder(scIntegrator_->getStepper()->getOrder());
    scMD->setIStep(0);
    scMD->setNFailures(0);
    scMD->setNRunningFailures(0);
    scMD->setNConsecutiveFailures(0);
    scMD->setOutput(false);
    scMD->setOutputScreen(false);

    TEUCHOS_TEST_FOR_EXCEPTION(!scMD->getIsSynced(), std::logic_error,
      "Error - StepperSubcycling<Scalar>::takeStep(...)\n"
      "        Subcycling requires the the solution is synced!\n"
      "        (i.e., x, xDot, and xDotDot at the same time level.\n");

    auto subcyclingState = rcp(new SolutionState<Scalar>( scMD,
                                           currentState->getX(),
                                           currentState->getXDot(),
                                           currentState->getXDotDot(),
                                           currentState->getStepperState(),
                                           currentState->getPhysicsState()));

    auto scSH = rcp(new Tempus::SolutionHistory<Scalar>());
    scSH->setName("Subcycling States");
    scSH->setStorageType(Tempus::STORAGE_TYPE_STATIC);
    scSH->setStorageLimit(3);
    scSH->addState(subcyclingState);

    scIntegrator_->setSolutionHistory(scSH);

    bool pass = scIntegrator_->advanceTime();

    RCP<SolutionState<Scalar> > scCS = scSH->getCurrentState();

    RCP<Thyra::VectorBase<Scalar> > x   = workingState->getX();
    RCP<Thyra::VectorBase<Scalar> > scX = scCS->getX();
    Thyra::V_V(x.ptr(),       *(scX));

    RCP<Thyra::VectorBase<Scalar> > xDot   = workingState->getXDot();
    if (xDot != Teuchos::null) {
      RCP<Thyra::VectorBase<Scalar> > scXDot = scCS->getXDot();
      Thyra::V_V(xDot.ptr(),       *(scXDot));
    }

    RCP<Thyra::VectorBase<Scalar> > xDotDot   = workingState->getXDotDot();
    if (xDotDot != Teuchos::null) {
      RCP<Thyra::VectorBase<Scalar> > scXDotDot = scCS->getXDotDot();
      Thyra::V_V(xDotDot.ptr(),       *(scXDotDot));
    }

    if (pass == true) workingState->setSolutionStatus(Status::PASSED);
    else              workingState->setSolutionStatus(Status::FAILED);
    workingState->setOrder(scCS->getOrder());
    scSH->clear();
    stepperSCObserver_->observeEndTakeStep(solutionHistory, *this);
  }
  return;
}


/** \brief Provide a StepperState to the SolutionState.
 *  This Stepper does not have any special state data,
 *  so just provide the base class StepperState with the
 *  Stepper description.  This can be checked to ensure
 *  that the input StepperState can be used by this Stepper.
 */
template<class Scalar>
Teuchos::RCP<Tempus::StepperState<Scalar> > StepperSubcycling<Scalar>::
getDefaultStepperState()
{
  Teuchos::RCP<Tempus::StepperState<Scalar> > stepperState =
    rcp(new StepperState<Scalar>(this->getStepperType()));
  return stepperState;
}


template<class Scalar>
void StepperSubcycling<Scalar>::describe(
  Teuchos::FancyOStream               &out,
  const Teuchos::EVerbosityLevel      verbLevel) const
{
  out << std::endl;
  Stepper<Scalar>::describe(out, verbLevel);

  out << "--- StepperSubcycling ---\n";
  out << "  stepperSCObserver = " << stepperSCObserver_ << std::endl;
  out << "  scIntegrator      = " << scIntegrator_ << std::endl;
  out << "-------------------------" << std::endl;
  scIntegrator_->getStepper()->describe(out, verbLevel);
}


template<class Scalar>
Teuchos::RCP<const Teuchos::ParameterList>
StepperSubcycling<Scalar>::getValidParameters() const
{
  TEUCHOS_TEST_FOR_EXCEPTION(true, std::logic_error,
    "Error - StepperSubcycling<Scalar>::getValidParameters()\n"
    "  is not implemented yet.\n");

  Teuchos::RCP<Teuchos::ParameterList> pl = Teuchos::parameterList();
  getValidParametersBasic(pl, this->getStepperType());
  pl->set<bool>("Use FSAL", true);
  pl->set<std::string>("Initial Condition Consistency", "Consistent");
  return pl;
}


} // namespace Tempus
#endif // Tempus_StepperSubcycling_impl_hpp

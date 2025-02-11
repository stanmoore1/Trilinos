//@HEADER
// ************************************************************************
//
//               ShyLU: Hybrid preconditioner package
//                 Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Alexander Heinlein (alexander.heinlein@uni-koeln.de)
//
// ************************************************************************
//@HEADER

#ifndef _FROSCH_SCHWARZPRECONDITIONER_DEF_HPP
#define _FROSCH_SCHWARZPRECONDITIONER_DEF_HPP

#include <FROSch_SchwarzPreconditioner_decl.hpp>


namespace FROSch {

    using namespace Teuchos;
    using namespace Xpetra;

    template <class SC,class LO,class GO,class NO>
    SchwarzPreconditioner<SC,LO,GO,NO>::SchwarzPreconditioner(ParameterListPtr parameterList,
                                                              CommPtr comm) :
    MpiComm_ (comm),
    ParameterList_ (parameterList),
    UseTranspose_ (false),
    IsInitialized_ (false),
    IsComputed_ (false),
    Verbose_ (comm->getRank()==0),
    LevelID_ (ParameterList_->get("Level ID",UN(1)))
    {

    }

    template <class SC,class LO,class GO,class NO>
    SchwarzPreconditioner<SC,LO,GO,NO>::~SchwarzPreconditioner()
    {

    }

    template <class SC,class LO,class GO,class NO>
    bool SchwarzPreconditioner<SC,LO,GO,NO>::isInitialized() const
    {
        return IsInitialized_; // TODO: Das hat noch keine Bedeutung
    }

    template <class SC,class LO,class GO,class NO>
    bool SchwarzPreconditioner<SC,LO,GO,NO>::isComputed() const
    {
        return IsComputed_; // TODO: Das hat noch keine Bedeutung
    }

    template <class SC,class LO,class GO,class NO>
    void SchwarzPreconditioner<SC,LO,GO,NO>::residual(const XMultiVector & X,
                                                      const XMultiVector & B,
                                                      XMultiVector& R) const 
    {
        SC one = Teuchos::ScalarTraits<SC>::one(), negone = -one;
        apply(X,R);
        R.update(one,B,negone);
    }

}

#endif

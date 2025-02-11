// Copyright 2002 - 2008, 2010, 2011 National Technology Engineering
// Solutions of Sandia, LLC (NTESS). Under the terms of Contract
// DE-NA0003525 with NTESS, the U.S. Government retains certain rights
// in this software.
//
 // Redistribution and use in source and binary forms, with or without
 // modification, are permitted provided that the following conditions are
 // met:
 // 
 //     * Redistributions of source code must retain the above copyright
 //       notice, this list of conditions and the following disclaimer.
 // 
 //     * Redistributions in binary form must reproduce the above
 //       copyright notice, this list of conditions and the following
 //       disclaimer in the documentation and/or other materials provided
 //       with the distribution.
 // 
//     * Neither the name of NTESS nor the names of its contributors
//       may be used to endorse or promote products derived from this
//       software without specific prior written permission.
//
 // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 // "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 // LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 // A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 // OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 // SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 // LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 // DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 // THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stk_util/command_line/ParseCommandLineArgs.hpp>

namespace stk {

bool insert_option(const Option& option, const std::string& value, stk::ParsedOptions& varMap)
{
    std::string theValue = value.empty() ? option.defaultValue : value;
    return varMap.insert(option.name, option.abbrev, theValue);
}

void make_sure_all_required_were_found(const OptionsSpecification& optionsDesc,
                                       const ParsedOptions& varMap)
{
  for(const Option& option : optionsDesc.get_options()) {
    ThrowRequireMsg(!option.isRequired || varMap.count(option.name) == 1,
                    "Error in stk::parse_command_line_args: "
                    << "required option '"<<dash_it(option.name)<<"' not found.");
  }
}

void parse_command_line_args(int argc, const char** argv,
                             const OptionsSpecification& optionsDesc,
                             stk::ParsedOptions& varMap)
{
  for(const Option& option : optionsDesc.get_options()) {
    if (!option.defaultValue.empty()) {
      insert_option(option, "", varMap);
    }
  }

  std::vector<bool> argHasBeenUsed(argc, false);
  for(int i=1; i<argc; ++i) {
    const char* arg = argv[i];
    if (arg == nullptr) {
      continue;
    }

    const bool isPositionalArg = (arg[0] != '-');
    if (isPositionalArg) {
      continue;
    }

    std::string optionKey = rm_dashes(std::string(arg));
    std::string value;
    bool argContainedEquals = false;
    size_t posOfEqualSign = optionKey.find("=");
    if (posOfEqualSign != std::string::npos) {
      argContainedEquals = true;
      value = optionKey.substr(posOfEqualSign+1);
      optionKey = optionKey.substr(0, posOfEqualSign);
    }

    const Option& option = optionsDesc.find_option(optionKey);
    if (option.name.empty()) {
      continue;
    }

    argHasBeenUsed[i] = true;

    if (!option.isFlag && !argContainedEquals) {
      ThrowRequireMsg(i < (argc-1), "stk::parse_command_line_args: reached end of command line "
                      <<" without finding value for option "<<dash_it(option.name));
      value = argv[i+1];
      argHasBeenUsed[i+1] = true;
      ++i;
    }
    insert_option(option, value, varMap);
  }

  if (optionsDesc.get_num_positional_options() > 0) {
    int positionalArgIndex = 0;
    for(int i=1; i<argc; ++i) {
      const bool isPositionalArg = !argHasBeenUsed[i];
      if (isPositionalArg) {
        const Option& option = optionsDesc.get_positional_option(positionalArgIndex);
        insert_option(option, std::string(argv[i]), varMap);
        ++positionalArgIndex;
      }
    }
  }

  if (varMap.count("help") == 0 && varMap.count("version") == 0) {
    make_sure_all_required_were_found(optionsDesc, varMap);
  }
}

}


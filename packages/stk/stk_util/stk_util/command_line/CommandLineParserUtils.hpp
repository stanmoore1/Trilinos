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

#ifndef STK_STK_UTIL_STK_UTIL_COMMAND_LINE_COMMANDLINEPARSERUTILS_HPP_
#define STK_STK_UTIL_STK_UTIL_COMMAND_LINE_COMMANDLINEPARSERUTILS_HPP_

#include <stk_util/stk_config.h>
#include <stk_util/parallel/Parallel.hpp>
#include <string>

namespace stk {

class CommandLineParserParallel;

std::string angle_it(const std::string &s);
std::string bracket_it(const std::string &s);
std::string dash_it(const std::string &s);
std::string rm_dashes(const std::string &s);
std::string get_substring_before_comma(const std::string& s);
std::string get_substring_after_comma(const std::string& s);
std::string get_quick_error(const std::string &execName, const std::string &quickExample);
std::string get_version(const std::string &executableName);
void parse_command_line(int argc,
                        const char** argv,
                        const std::string& quickExample,
                        const std::string& longExample,
                        stk::CommandLineParserParallel& commandLine,
                        MPI_Comm comm);
namespace parallel {
void print_and_exit(const std::string &msg, MPI_Comm comm);
void require(bool requirement, const std::string &msg, MPI_Comm comm);
bool does_file_exist(const std::string& filename);
void require_file_exists(const std::string& inFile, const std::string& execName, const std::string& quickExample, MPI_Comm comm);
}


}

#endif /* STK_STK_UTIL_STK_UTIL_COMMAND_LINE_COMMANDLINEPARSERUTILS_HPP_ */

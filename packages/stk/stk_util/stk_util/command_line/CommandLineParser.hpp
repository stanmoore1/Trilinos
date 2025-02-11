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

#ifndef STK_UTIL_ENVIRONMENT_COMMANDLINEPARSER_HPP
#define STK_UTIL_ENVIRONMENT_COMMANDLINEPARSER_HPP

#include <stk_util/command_line/OptionsSpecification.hpp>
#include <stk_util/command_line/ParsedOptions.hpp>
#include <stk_util/command_line/ParseCommandLineArgs.hpp>
#include <iostream>
#include <string>

namespace stk {

struct CommandLineOption
{
    std::string name;
    std::string abbreviation;
    std::string description;
};

class CommandLineParser
{
public:
    enum ParseState { ParseComplete, ParseError, ParseHelpOnly, ParseVersionOnly };
    CommandLineParser() : CommandLineParser("Options") {}
    explicit CommandLineParser(const std::string &usagePreamble)
    : optionsSpec(usagePreamble),
      parsedOptions(),
      positionalIndex(0)
    {
        add_flag("help,h", "display this help message and exit");
        add_flag("version,v", "display version information and exit");
    }

    void add_flag(const std::string &option, const std::string &description)
    {
        optionsSpec.add_options()
          (option, description);
    }

    template <typename ValueType>
    void add_required_positional(const CommandLineOption &option)
    {
        add_required<ValueType>(option, positionalIndex);
        ++positionalIndex;
    }

    template <typename ValueType>
    void add_optional_positional(const CommandLineOption &option, const ValueType &def)
    {
        add_optional<ValueType>(option, def, positionalIndex);
        ++positionalIndex;
    }

    template <typename ValueType>
    void add_required(const CommandLineOption &option, int position = -2)
    {
        const bool isFlag = false;
        const bool isRequired = true;
        optionsSpec.add_options()
          (get_option_spec(option), isFlag, isRequired, option.description, position);
    }

    template <typename ValueType>
    void add_optional(const CommandLineOption &option,
                      const ValueType &defaultValue,
                      int position = -2)
    {
        add_optional(get_option_spec(option), option.description, defaultValue, position);
    }

    template <typename ValueType>
    void add_optional(const std::string &option, const std::string &description,
                      const ValueType &defaultValue, int position = -2)
    {
        const bool isFlag = false;
        const bool isRequired = false;
        optionsSpec.add_options()
          (option, description, defaultValue, isFlag, isRequired, position);
    }

    std::string get_usage() const
    {
        std::ostringstream os;
        os << optionsSpec << std::endl;
        return os.str();
    }

    ParseState parse(int argc, const char *argv[])
    {
        ParseState state = ParseError;
        try
        {
            stk::parse_command_line_args(argc, argv, optionsSpec, parsedOptions);
            if(is_option_provided("help"))
                return ParseHelpOnly;
            if(is_option_provided("version"))
                return ParseVersionOnly;

            state = ParseComplete;
        }
        catch(std::exception &e)
        {
            print_message(e.what());
        }
        return state;
    }

    bool is_option_provided(const std::string &option) const
    {
        return parsedOptions.count(option) > 0;
    }

    bool is_empty() const
    {
        return parsedOptions.empty();
    }

    template <typename ValueType>
    ValueType get_option_value(const std::string &option) const
    {
        return parsedOptions[option].as<ValueType>();
    }

protected:
    std::string get_option_spec(const CommandLineOption &option)
    {
        return option.name + "," + option.abbreviation;
    }

    virtual void print_message(const std::string &msg)
    {
        std::cerr << msg << std::endl;
    }

    stk::OptionsSpecification optionsSpec;
    stk::ParsedOptions parsedOptions;
    int positionalIndex;
};

}

#endif //STK_UTIL_ENVIRONMENT_COMMANDLINEPARSER_HPP

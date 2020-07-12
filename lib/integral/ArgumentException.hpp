//
//  ArgumentException.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef integral_ArgumentException_hpp
#define integral_ArgumentException_hpp

#include <cstddef>
#include <stdexcept>
#include <exception>
#include <string>
#include <lua.hpp>

namespace integral {
    class ArgumentException : public std::invalid_argument {
    public:
        // necessary because of the user-defined destructor
        ArgumentException(const ArgumentException &) = default;
        ArgumentException(ArgumentException &) = default;
        ArgumentException(ArgumentException &&) = default;

        // this is necessary (see definition)
        ~ArgumentException() override;

        static ArgumentException createTypeErrorException(lua_State *luaState, int index, const std::string &expectedTypeName);

        inline ArgumentException(lua_State *luaState, int index, const std::string &extraMessage);
        inline ArgumentException(lua_State *luaState, std::size_t maximumNumberOfArguments, std::size_t actualNumberOfArguments);

    private:
        static bool findField(lua_State *luaState, int index, int level);
        static bool pushGlobalFunctionName(lua_State *L, lua_Debug *debugInfo);
        static std::string getExceptionMessage(lua_State *luaState, int index, const std::string &extraMessage);
        static std::string getExceptionMessage(lua_State *luaState, std::size_t maximumNumberOfArguments, std::size_t actualNumberOfArguments);
    };

    //--


    inline ArgumentException::ArgumentException(lua_State *luaState, int index, const std::string &extraMessage) : std::invalid_argument(getExceptionMessage(luaState, index, extraMessage)) {}

    inline ArgumentException::ArgumentException(lua_State *luaState, std::size_t maximumNumberOfArguments, std::size_t actualNumberOfArguments) : std::invalid_argument(getExceptionMessage(luaState, maximumNumberOfArguments, actualNumberOfArguments)) {}
}


#endif

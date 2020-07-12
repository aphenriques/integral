//
//  utility.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_utility_hpp
#define integral_utility_hpp

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <string>
#include <tuple>
#include <utility>
#include <lua.hpp>

namespace integral {
    namespace utility {
        std::string getStackString(lua_State *luaState);

        int printStack(lua_State *luaState);

        // stack argument: table
        void setHelp(lua_State *luaState, const char *field, const char *fieldDescription);

        // stack argument: table | ?
        void setWithHelp(lua_State *luaState, const char *field, const char *fieldDescription);

        void pushNameAndValueList(lua_State *luaState, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);

        // stack argument: table
        void setNameAndValueListWithHelp(lua_State *luaState, const char *field, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
    }
}

#endif

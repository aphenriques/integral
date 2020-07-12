//
//  algorithm.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <algorithm>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <lua.hpp>
#include <integral/integral.hpp>

namespace {
    std::vector<int> getTransformed(std::vector<int> vectorCopy, const integral::LuaFunctionArgument &luaFunctionArgument) {
        // vectorCopy is a copy of the lua vector
        // the lua table argument is not modified
        std::for_each(vectorCopy.begin(), vectorCopy.end(), [&luaFunctionArgument](int &element) -> void {
            element = luaFunctionArgument.call<int>(element);
        });
        return vectorCopy;
    }
}

extern "C" {
    LUALIB_API int luaopen_libalgorithm(lua_State *luaState) {
        try {
            // module table
            lua_newtable(luaState);
            integral::setFunction(luaState, "getTransformed", getTransformed);
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[luafunction_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[luafunction_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

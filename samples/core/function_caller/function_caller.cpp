//
//  function_caller.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

int main() {
    lua_State *luaState = nullptr;

    try {
        luaState = luaL_newstate();
        if (luaState == nullptr) {
            return 1;
        }

        integral::pushFunction(luaState, [](int x) -> int {
            return 2*x;
        });
        lua_setglobal(luaState, "cppDouble");

        luaL_dostring(luaState, "function luaMultiply(x, y) return x*y end");

        lua_getglobal(luaState, "cppDouble");
        // like in lua api, integral::call pops the function on top of the stack
        std::cout << "cppDouble(21) = " << integral::call<int>(luaState, 21) << std::endl;

        lua_getglobal(luaState, "luaMultiply");
        std::cout << "luaMultiply(-21, 2) = " << integral::call<int>(luaState, -21, 2) << std::endl;

        // error handling
        try {
            lua_getglobal(luaState, "luaMultiply");
            std::cout << "[equivalent] luaMultiply(-21):" << std::endl;
            // integral::call is done in protected mode. Throws an exception in case of error
            integral::call<int>(luaState, -21);
        } catch (const integral::CallerException &exception) {
            std::cout << "expected error: " << exception.what() << std::endl;
        }

        // error handling
        try {
            lua_getglobal(luaState, "cppDouble");
            std::cout << "[equivalent] cppDouble(\"asd\"):" << std::endl;
            // integral::call is done in protected mode. Throws an exception in case of error
            integral::call<int>(luaState, "asd");
        } catch (const integral::CallerException &exception) {
            std::cout << "expected error: " << exception.what() << std::endl;
        }

        lua_close(luaState);

        return 0;
    } catch (const std::exception &exception) {
        std::cout << "[function_caller sample] " << exception.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception thrown" << std::endl;
    }

    lua_close(luaState);
    return 1;
}

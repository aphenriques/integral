//
//  state.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <lua.hpp>
#include <integral/integral.hpp>

static const char * const luaTestCode = "print('hello from string sample code')";

int main() {
    try {
        std::cout << "State:\n";
        integral::State luaState;
        luaState.openLibs();
        luaState.doFile("sample.lua");
        luaState.doString(luaTestCode);

        std::cout << "StateView:\n";
        std::unique_ptr<lua_State, decltype(&lua_close)> luaStateUniquePtr(luaL_newstate(), &lua_close);
        // lua_State pointer ownership is NOT transfered to luaStateView (it remains with luaStateUniquePtr)
        integral::StateView luaStateView(luaStateUniquePtr.get()); 
        luaStateView.openLibs();
        luaStateView.doFile("sample.lua");
        luaStateView.doString(luaTestCode);

        try {
            integral::StateView(nullptr);
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doString("invalid_statement");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doFile("nonexistentFile");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[state] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

//
//  lua_function_argument.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <algorithm>
#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

std::vector<int> getTransformed(std::vector<int> vectorCopy, const integral::LuaFunctionArgument &luaFunctionArgument) {
    // vectorCopy is a copy of the lua vector
    // the lua table argument is not modified
    std::for_each(vectorCopy.begin(), vectorCopy.end(), [&luaFunctionArgument](int &element) -> void {
        element = luaFunctionArgument.call<int>(element);
    });
    return vectorCopy;
}

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        integral::pushCopy(luaState.getLuaState(), 1);
        integral::get<double>(luaState.getLuaState(), -1);

        luaState["getResult"].setFunction([](int x, int y, const integral::LuaFunctionArgument &function) {
            return function.call<int>(x, y);
        });
        luaState.doString("print(getResult(-1, 1, math.min))");

        luaState["getTransformed"].setFunction(getTransformed);
        luaState.doString("t = getTransformed({1, 2}, function(element) return 2*element end)\n"
                          "print(t[1])\n"
                          "print(t[2])");

        try {
            luaState.doString("print(getResult(-1, 1, function() end))");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doString("getTransformed({{}, 2}, function(element) return 2*element end)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[lua_function_argument] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

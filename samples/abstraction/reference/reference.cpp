//
//  reference.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020, 2022 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <iomanip>
#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        std::cout << "cpp: a == nil -> " << std::boolalpha << luaState["a"].isNil() << '\n';
        luaState.doString("a = 42");
        std::cout << "cpp: a = " << luaState["a"].get<int>() << '\n';
        std::cout << "cpp: a == nil -> " << luaState["a"].isNil() << '\n';

        luaState["b"].set("forty two");
        luaState.doString("print('lua: b = ' .. b)\n"
                          "c = .42");
        double c = luaState["c"];
        std::cout << "cpp: c = " << c << '\n';

        luaState["d"] = "quarenta e dois";
        luaState.doString("print('lua: d = ' .. d)\n"
                          "t = {'x', {pi = 3.14}}");
        std::cout << "cpp: t[1] = " << luaState["t"][1].get<std::string>() << '\n';
        std::cout << "cpp: t[2].pi = " << luaState["t"][2]["pi"].get<double>() << '\n';
        luaState["t"]["key"] = "value";
        luaState.doString("print('lua: t.key = ' .. t.key)");

        try {
            luaState["x"].get<std::string>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            luaState["t"]["x"]["x"] = true;
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            [[maybe_unused]] int x = luaState["x"];
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[reference] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

//
//  function.cpp
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

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

double getSum(double x, double y) {
    return x + y;
}

double luaGetSum(lua_State *luaState) {
    integral::pushCopy(luaState, integral::get<double>(luaState, 1) + integral::get<double>(luaState, 2));
    return 1;
}

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["getSum"].setFunction(getSum);
        luaState.doString("print('getSum(1, -.2) = ' .. getSum(1, -.2))");

        luaState["luaGetSum"].setLuaFunction(luaGetSum);
        luaState.doString("print('luaGetSum(1, -.2) = ' .. luaGetSum(1, -.2))");

        luaState["printHello"].setFunction([]{
            std::puts("hello!");
        });
        luaState.doString("printHello()");

        luaState["getQuoted"].setFunction([](const std::string &string) {
            return std::string("\"") + string + '"';
        });
        luaState.doString("print('getQuoted(\"quoted\") = ' .. getQuoted('quoted'))");

        luaState["luaGetQuoted"].setLuaFunction([](lua_State *lambdaLuaState) {
            integral::push<std::string>(lambdaLuaState, std::string("\"") + integral::get<const char *>(lambdaLuaState, 1) + '"');
            return 1;
        });
        luaState.doString("print('luaGetQuoted(\"quoted\") = ' .. luaGetQuoted('quoted'))");

        try {
            luaState.doString("getSum(1, 2, 3)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doString("getSum(1)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doString("getSum(1, 'two')");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState.doString("luaGetSum(1, 'two')");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState["throw"].setFunction([]{
                throw std::runtime_error("throw exception");
            });
            luaState.doString("throw()");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        try {
            luaState["luaThrow"].setLuaFunction([](lua_State *) {
                throw std::runtime_error("luaThrow exception");
                return 0;
            });
            luaState.doString("luaThrow()");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[function] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

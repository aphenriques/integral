//
//  function.cpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
//  aphenriques (at) outlook (dot) com
//
//  This file is part of integral.
//
//  integral is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  integral is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with integral.  If not, see <http://www.gnu.org/licenses/>.
//

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <lua.hpp>
#include <integral.hpp>

double getSum(double x, double y) {
    return x + y;
}

double luaGetSum(lua_State *luaState) {
    integral::pushCopy(luaState, integral::get<double>(luaState, 1) + integral::get<double>(luaState, 2));
    return 1;
}

int main(int argc, char* argv[]) {
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

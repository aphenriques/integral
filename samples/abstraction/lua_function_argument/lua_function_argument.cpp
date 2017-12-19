//
//  lua_function_argument.cpp
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

int main(int argc, char* argv[]) {
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

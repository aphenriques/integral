//
//  function_caller.cpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016, 2017  André Pereira Henriques
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

//
//  state.cpp
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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
#include <memory>
#include <string>
#include <lua.hpp>
#include "integral.hpp"
#include "utility.hpp"

static const char * const luaTestCode = R"(
--require("usr")
print(x)
y = {{1, 2}, x = 'string'}
)";

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        try {
            std::cout << lua_typename(luaState.getLuaState(), LUA_TLIGHTUSERDATA) << std::endl;
            luaState.openLibs();
            luaState.doFile("test.lua");
            luaState["x"].set(42);
            luaState.doString(luaTestCode);
            int x = luaState["x"].get<int>();
            std::cout << "c++: " << x << std::endl;
            std::cout << "c++: " << luaState["y"]["x"].get<const char*>() << std::endl;
            std::cout << "c++: " << luaState["y"][1][2].get<unsigned>() << std::endl;
            auto xRef = luaState["x"];
            std::cout << "c++: " << xRef.get<int>() << std::endl;
            luaState["get42"].set(integral::FunctionWrapper<int()>([]() -> int {
                return 42;
            }));
            luaState.doString(R"(print("lua get42(): " .. get42()))");
            luaState.doString("print(y.x)");
            integral::utility::printStack(luaState.getLuaState());
            std::cout << "fim" << std::endl;
            return EXIT_SUCCESS;
        } catch (const std::exception &exception) {
            std::cerr << "exception: " << exception.what() << std::endl;
        } catch (...) {
            std::cerr << "unknown exception thrown" << std::endl;
        }
        integral::utility::printStack(luaState.getLuaState());
        return EXIT_FAILURE;
    } catch (const std::exception &exception) {
        std::cerr << "exception: " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

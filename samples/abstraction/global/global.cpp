//
//  global.cpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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
#include <iostream>
#include <integral/integral.hpp>

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();
        luaState["x"] = 42;
        luaState["y"] = integral::Global()["x"]; // equivalent to "y = x" in lua
        luaState.doString("print(y)"); // prints "42"
        luaState["global"] = integral::Global(); // equivalent to "global = _G" in lua
        luaState.doString("print(global.y)"); // prints "42"
        luaState["t"] = integral::Table().set("x", integral::Global()["global"]["y"]); // equivalent to "t = {x = global.y}" in lua
        luaState.doString("print(t.x)"); // prints "42"
        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[global] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

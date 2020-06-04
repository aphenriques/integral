//
//  pusher.cpp
//  integral
//
//  Copyright (C) 2020  André Pereira Henriques
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
#include <lua.hpp>
#include <integral/integral.hpp>

void pushModule(lua_State *luaState) {
    integral::push<integral::Table>(luaState);
    integral::setFunction(
        luaState,
        "printHello",
        [] {
            std::cout << "Hello!" << std::endl;
        }
    );
}

int main() {
    try {
        integral::State luaState;
        luaState["module"] = integral::Pusher(pushModule);
        luaState.doString("module.printHello()");
        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[pusher] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

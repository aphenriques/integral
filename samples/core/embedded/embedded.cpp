//
//  embedded.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016, 2017  André Pereira Henriques
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

class Object {
public:
    void printMessage() const {
        std::cout << "Object " << this << " message!" << std::endl;
    }
};

int main(int argc, char * argv[]) {
    lua_State *luaState = nullptr;

    try {
        luaState = luaL_newstate();
        if (luaState == nullptr) {
            return 1;
        }

        integral::pushClassMetatable<Object>(luaState);
        integral::setConstructor<Object()>(luaState, "new");
        // lua function name need not be the same as the C++ function name
        integral::setFunction(luaState, "print", &Object::printMessage);
        lua_setglobal(luaState, "Object");

        luaL_dostring(luaState, "local object = Object.new()\n"
                                "object:print()");

        lua_close(luaState);
        return 0;
    } catch (const std::exception &exception) {
        std::cout << "[embedded sample] " << exception.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception thrown" << std::endl;
    }

    lua_close(luaState);
    return 1;
}

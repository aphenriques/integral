//
//  reference.cpp
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

#include <functional>
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
        luaL_openlibs(luaState);

        integral::pushClassMetatable<Object>(luaState);
        integral::setFunction(luaState, "print", &Object::printMessage);

        // 'synthetic inheritance' can be viewed as a transformation from composition in c++ to inheritance in lua
        integral::defineReferenceWrapperInheritance<Object>(luaState);
        // alternative expression:
        //integral::defineInheritance(luaState, [](std::reference_wrapper<Object> *objectReference) -> Object * {
            //return &objectReference->get();
        //});

        lua_pop(luaState, 1);

        std::cout << "C++:" << std::endl;
        Object object;
        object.printMessage();

        integral::push<std::reference_wrapper<Object>>(luaState, object);
        lua_setglobal(luaState, "objectReference");

        luaL_dostring(luaState, "print('lua:')\n"
                                "objectReference:print()");

        lua_close(luaState);
        return 0;
    } catch (const std::exception &exception) {
        std::cout << "[reference sample] " << exception.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception thrown" << std::endl;
    }

    lua_close(luaState);
    return 1;
}

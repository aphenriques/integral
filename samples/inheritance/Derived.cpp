//
//  Derived.cpp
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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
#include <string>
#include <lua.hpp>
#include "integral.h"

class BaseOfBase1 {
public:
    void baseOfBase1Method() const {
        std::cout << "baseOfBase1Method" << std::endl;
    }
};

class Base1 : public BaseOfBase1 {
public:
    void base1Method() const {
        std::cout << "base1Method" << std::endl;
    }
};

class Base2 {
public:
    void base2Method() const {
        std::cout << "base2Method" << std::endl;
    }
};

class Derived : public Base1, public Base2 {
public:
    void derivedMethod() const {
        std::cout << "derivedMethod" << std::endl;
    }
};


extern "C" {
    LUALIB_API int luaopen_libDerived(lua_State *luaState) {
        try {
            // base and derived class registering could have been done in any order
            // besides what is stack concerned, integral does not rely on the order in which its functions are called
            // multiple inheritance is supported
            // inherited methods (and typeFunctions) are also inherited to derived classes

            integral::pushClassMetatable<Base2>(luaState);
            integral::setFunction(luaState, "base2Method", &Base2::base2Method);
            lua_pop(luaState, 1);

            integral::defineInheritance<Derived, Base2>(luaState);

            integral::pushClassMetatable<Base1>(luaState);
            integral::setFunction(luaState, "base1Method", &Base1::base1Method);
            lua_pop(luaState, 1);

            integral::pushClassMetatable<Derived>(luaState);
            integral::setFunction(luaState, "derivedMethod", &Derived::derivedMethod);
            integral::setConstructor<Derived>(luaState, "new");

            integral::defineInheritance<Derived, Base1>(luaState);

            integral::pushClassMetatable<BaseOfBase1>(luaState);
            integral::setFunction(luaState, "baseOfBase1Method", &BaseOfBase1::baseOfBase1Method);
            lua_pop(luaState, 1);

            integral::defineInheritance<Base1, BaseOfBase1>(luaState);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[inheritance sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[inheritance sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


//
//  Derived.cpp
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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

void callAllTypes(const BaseOfBase1 &baseOfBase1, const Base1 &base1, const Base2 &base2, const Derived &derived) {
    baseOfBase1.baseOfBase1Method();
    base1.base1Method();
    base2.base2Method();
    derived.derivedMethod();
}

extern "C" {
    LUALIB_API int luaopen_libDerived(lua_State *luaState) {
        try {

            integral::pushClassMetatable<Derived>(luaState);
            integral::setConstructor<Derived>(luaState, "new");
            integral::setFunction(luaState, "baseOfBase1Method", &Derived::baseOfBase1Method);
            integral::setFunction(luaState, "base1Method", &Derived::base1Method);
            integral::setFunction(luaState, "base2Method", &Derived::base2Method);
            integral::setFunction(luaState, "derivedMethod", &Derived::derivedMethod);
            integral::setFunction(luaState, "callAllTypes", callAllTypes);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[rtti_polymorphism sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[rtti_polymorphism sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


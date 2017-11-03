//
//  inheritance.cpp
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
#include <iostream>
#include <lua.hpp>
#include <integral.hpp>

class BaseOfBase1 {
public:
    void baseOfBase1Method() const {
        std::puts("baseOfBase1Method");
    }
};

class Base1 : public BaseOfBase1 {
public:
    void base1Method() const {
        std::puts("base1Method");
    }
};

class Base2 {
public:
    void base2Method() const {
        std::puts("base2Method");
    }
};

class Derived : public Base1, public Base2 {
public:
    void derivedMethod() const {
        std::puts("derivedMethod");
    }
};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;

        luaState["BaseOfBase1"] = integral::ClassMetatable<BaseOfBase1>()
                                  .setConstructor<BaseOfBase1()>("new")
                                  .setFunction("baseOfBase1Method", &BaseOfBase1::baseOfBase1Method);
        luaState["Base1"] = integral::ClassMetatable<Base1>()
                            .setConstructor<Base1()>("new")
                            .setFunction("base1Method", &Base1::base1Method)
                            .setBaseClass<BaseOfBase1>();
        luaState["Base2"] = integral::ClassMetatable<Base2>()
                            .setConstructor<Base2()>("new")
                            // .setBaseClass<BaseOfBase1>() // generates compile error
                            .setFunction("base2Method", &Base2::base2Method);
        luaState["Derived"] = integral::ClassMetatable<Derived>()
                            .setConstructor<Derived()>("new")
                            .setFunction("derivedMethod", &Derived::derivedMethod)
                            // .setBaseClass<BaseOfBase1>() // valid statement. BaseOfBase1 need not be defined as base class of Base1 in Base1 metatable
                            .setBaseClass<Base1>()
                            .setBaseClass<Base2>();
        luaState.doString("derived = Derived.new()\n"
                          "derived:base1Method()\n" // prints "base1Method"
                          "derived:base2Method()\n" // prints "base2Method"
                          "derived:baseOfBase1Method()\n" // prints "baseOfBase1Method"
                          "derived:derivedMethod()"); // prints "derivedMethod"

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[inheritance] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

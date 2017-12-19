//
//  polymorphism.cpp
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
#include <integral/integral.hpp>

class DiamondBase {};

class BaseOfBase1 {};

class Base1 : public BaseOfBase1, public DiamondBase {
public:
    void base1Method() {
        std::puts("base1Method");
    }
};

class Base2 : public DiamondBase {};

class Derived : public Base1, public Base2 {};

void callBaseOfBase1(const BaseOfBase1 &) {
    std::puts("BaseOfBase1");
}

void callBase1(const Base1 &) {
    std::puts("Base1");
}

void callBase2(const Base2 &) {
    std::puts("Base2");
}

void callDerived(const Derived &) {
    std::puts("Derived");
}

class Other {};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;

        luaState["Derived"] = integral::ClassMetatable<Derived>().setConstructor<Derived()>("new");
        luaState["callBaseOfBase1"].setFunction(callBaseOfBase1);
        luaState["callBase1"].setFunction(callBase1);
        luaState["callBase2"].setFunction(callBase2);
        luaState["callDerived"].setFunction(callDerived);
        luaState.doString("derived = Derived.new()\n"
                          "callBaseOfBase1(derived)\n"
                          "callBase1(derived)\n"
                          "callBase2(derived)\n"
                          "callDerived(derived)");

        luaState["Base1"] = integral::ClassMetatable<Base1>().setConstructor<Base1()>("new");
        try {
            luaState.doString("callDerived(Base1.new())");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        luaState["Base1"]["base1Method"].setFunction(&Base1::base1Method);
        luaState.doString("Base1.base1Method(derived)");
        try {
            // base classes methods are not automatically bound to derived types class metatables
            // see the inheritance sample for methods inheritance
            luaState.doString("derived:base1Method()");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        luaState["Other"] = integral::ClassMetatable<Other>().setConstructor<Other()>("new");
        try {
            luaState.doString("callDerived(Other.new())");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        try {
            luaState["callDiamondBase"].setFunction([](const DiamondBase &){});
            // ambiguous polymorphism :"diamond problem"
            luaState.doString("callDiamondBase(derived)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[polymorphism] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

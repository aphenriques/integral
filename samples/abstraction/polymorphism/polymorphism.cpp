//
//  polymorphism.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

int main() {
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

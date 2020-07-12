//
//  inheritance.cpp
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

int main() {
    try {
        integral::State luaState;

        luaState["BaseOfBase1"] = integral::ClassMetatable<BaseOfBase1>()
                                  .setConstructor<BaseOfBase1()>("new")
                                  .setFunction("baseOfBase1Method", &BaseOfBase1::baseOfBase1Method);
        luaState["Base1"] = integral::ClassMetatable<Base1>()
                            .setConstructor<Base1()>("new")
                            .setFunction("base1Method", &Base1::base1Method)
                            .setBaseClass<BaseOfBase1>();
        // alternative expression to .setBaseClass<BaseOfBase1>():
        // luaState.defineInheritance<Base1, BaseOfBase1>()
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

//
//  Derived.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <functional>
#include <iostream>
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>

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
            integral::setConstructor<Derived()>(luaState, "new");

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


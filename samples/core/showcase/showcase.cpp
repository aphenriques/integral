//
//  showcase.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2015, 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <iostream>
#include <string>
#include <functional>
#include <lua.hpp>
#include <integral/integral.hpp>

class Base {
public:
    double number_;
    Base(double number) : number_(number) {}
    double getSum(float number) const {
        return number_ + number;
    }
};


class Derived : public Base {
public:
    const std::string string_;

    Derived(double number, const char * string) : Base(number), string_(string) {}

    std::string getConcatenated(const std::string &string) const {
        return string_ + string;
    }
};

Base getSumBase(const Base &base1, const Base &base2) {
    return Base(base1.number_ + base2.number_);
}

extern "C" {
    // This sample does NOT show all integral features (only part of its capability)
    LUALIB_API int luaopen_libshowcase(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            lua_pushstring(luaState, "Base");
            // stack: table (module table) | string (Base class metatable field name on module table)

            integral::pushClassMetatable<Base>(luaState);
            // stack: table (module table) | string (Base class metatable field name on module table) | metatable (Base class metatable)

            integral::setConstructor<Base(double)>(luaState, "new");
            integral::setGetter(luaState, "getNumber", &Base::number_);
            integral::setSetter(luaState, "setNumber", &Base::number_);

            // Automatic conversion between c++ floating-point types (std::is_floating_point) and lua number for function return value and arguments
            // The same applies for integral types (std::is_integral)
            integral::setFunction(luaState, "getSum", &Base::getSum);

            // Setting methods, constructors and accessors does not change the stack
            // stack: table (module table) | string (Base class metatable field name on module table) | metatable (Base class metatable)
            lua_rawset(luaState, -3);
            // stack: table (module table)

            lua_pushstring(luaState, "Derived");
            // stack: table (module table) | string (Derived class metatable field name on module table)

            // Defining inheritance does not change the stack
            integral::defineInheritance<Derived, Base>(luaState);

            integral::pushClassMetatable<Derived>(luaState);
            // stack: table (module table) | string (Derived class metatable field name on module table) | metatable (Derived class metatable)

            integral::setConstructor<Derived(double, const char *)>(luaState, "new");
            integral::setGetter(luaState, "getString", &Derived::string_);

            // Automatic conversion between lua string and "[cv] std::string" or "const char *"
            integral::setFunction(luaState, "getConcatenated", &Derived::getConcatenated);

            // Method definition with lambda:
            integral::setFunction(luaState, "__tostring", [](const Derived &derived) -> std::string {
                return std::to_string(derived.number_) + " | " + derived.string_;
            });

            // Lua functions can be registered. Similarly to luaL_setfuncs (lua_CFunction). But, in integral, they can also be functors, and handle exceptions and invalid arguments (with integral::get) graciously
            integral::setLuaFunction(luaState, "getNumberAndString", [](lua_State * lambdaLuaState) -> int {
                Derived &derived = integral::get<Derived>(lambdaLuaState, 1);
                integral::push<double>(lambdaLuaState, derived.number_);
                integral::push<std::string>(lambdaLuaState, derived.string_);
                return 2; // 2 return values
            });

            // Setting methods, constructors and accessors does not change the stack
            // stack: table (module table) | string (Derived class metatable field name on module table) | metatable (Derived class metatable)
            lua_rawset(luaState, -3);
            // stack: table (module table)

            // A Class metatable can be pushed into stack, have some methods registered, then popped, and pushed again later for more methods registration

            // Module functions
            integral::setFunction(luaState, "getSumBase", &getSumBase);

            integral::setFunction(luaState, "throwCppException", [](const char * message) {
                throw std::runtime_error(message);
            });

            // Do not forget the return value (number of values to be returned from stack)!
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[showcase sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[showcase sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

//
//  Object.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <sstream>
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>

class Persistence {
public:
    std::string string_;
    double number_;
};

class Object {
public:
    std::string string_;
    double number_;

    Object(const std::string &string, double number) : string_(string), number_(number) {}
    Object(const Persistence &persistence) : string_(persistence.string_), number_(persistence.number_) {}

    void setStringAndNumber(const char *string, double number) {
        string_ = string;
        number_ = number;
    }

    void setStringAndNumber(const Persistence &persistence) {
        string_ = persistence.string_;
        number_ = persistence.number_;
    }

    Persistence getPersistence() const {
        return Persistence{string_, number_};
    }
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);

            // functions can be lambdas
            integral::setFunction(luaState, "toString", [](const Object &object) -> std::string {
                std::stringstream ss;
                ss << "Object " << &object << ": string_ = \"" << object.string_ << "\"; number_ = " << object.number_;
                return ss.str();
            });

            // overloading is not supported, so diferent constructors names must be provided
            // assigning a registered constructor name will override it
            integral::setConstructor<Object(const std::string &, double)>(luaState, "new");
            integral::setConstructor<Object(const Persistence &)>(luaState, "newFromPersistence");

            // static_cast is necessary for function overload ambiguity resolving
            // const char * and std::string are seamlessly converted to and from lua string type
            integral::setFunction(luaState, "setStringAndNumber", static_cast<void(Object::*)(const char *, double)>(&Object::setStringAndNumber));

            // function overload is not supported in integral. So it is necessary to register the overload with a diferent name
            // registering a different function with the same name of a previously registered one will override it
            integral::setFunction(luaState, "setStringAndNumberFromPersistence", static_cast<void(Object::*)(const Persistence &)>(&Object::setStringAndNumber));

            // Persistence class metatable need NOT be manually registered. Every type is automatically managed by integral
            integral::setFunction(luaState, "getPersistence", &Object::getPersistence);

            // getter (by value only) and setter functions
            integral::setGetter(luaState, "getNumber", &Object::number_);
            integral::setSetter(luaState, "setNumber", &Object::number_);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[class sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[class sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


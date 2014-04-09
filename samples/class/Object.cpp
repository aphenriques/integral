//
//  Object.cpp
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

#include <sstream>
#include <string>
#include <functional>
#include <lua.hpp>
#include "integral.h"

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

            // functions and constructor can be registered in any order.

            // functions can be lambdas
            integral::setFunction(luaState, "toString", std::function<std::string(const Object &)>([](const Object &object) -> std::string {
                std::stringstream ss;
                ss << "Object " << &object << ": string_ = \"" << object.string_ << "\"; number_ = " << object.number_;
                return ss.str();
            }));

            integral::setConstructor<Object, std::string, double>(luaState, "new");

            // static_cast is necessary for name overload ambiguity resolving
            // const char * and std::string are seamlessly converted to and from lua string type
            integral::setFunction(luaState, "setStringAndNumber", static_cast<void(Object::*)(const char *, double)>(&Object::setStringAndNumber));

            // function overload is not supported in integral (registering a different function with the same name of other will override the previous definition). So it is necessary to register the overload with a diferent name
            integral::setFunction(luaState, "setStringAndNumberFromPersistence", static_cast<void(Object::*)(const Persistence &)>(&Object::setStringAndNumber));

            // Persistence need not be manually declared. Every type is automatically managed by integral
            integral::setFunction(luaState, "getPersistence", &Object::getPersistence);

            // getter (by value only) and setter functions
            integral::setCopyGetter(luaState, "getNumber", &Object::number_);
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


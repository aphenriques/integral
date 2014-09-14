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

#include <functional>
#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

class Object {
public:
    std::string string_;

    Object(const std::string &string) : string_(string) {}

    void testDefault(int dummy) const {
        std::cout << "Object::string_: \"" << string_ << "\" | dummy argument: " << dummy << std::endl;
    }
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);

            // default argument in constructor
            integral::setConstructor<Object, const std::string &>(luaState, "new", integral::DefaultArgument<std::string, 1>("default Object.new argument"));

            // DefaultArgument template parameters are checked at compile time. If there is type or index inconsistency, there will be compilation error. E.g:
            // will fail:
            //integral::setConstructor<Object, const std::string &>(luaState, "new", integral::DefaultArgument<std::string, 2>("default Object.new argument"));
            // will fail:
            //integral::setConstructor<Object, const std::string &>(luaState, "new", integral::DefaultArgument<double, 1>(42.0));
            // will fail:
            //integral::setConstructor<Object, const std::string &>(luaState, "new", integral::DefaultArgument<std::string, 1>("default Object.new argument"), integral::DefaultArgument<std::string, 1>("wrong redefinition of default argument"));

            integral::setCopyGetter(luaState, "getString", &Object::string_);

            // default argument in function
            integral::setFunction(luaState, "testDefault", &Object::testDefault, integral::DefaultArgument<Object, 1>("default Object.testDefault Object argument"), integral::DefaultArgument<int, 2>(42));

            integral::setFunction(luaState, "testDefault2", std::function<void(double, const std::string &)>([](double number, const std::string &string) -> void {
                std::cout << "Object.testDefault2 - number: " << number << "; string: \"" << string << "\"" << std::endl;
            }), integral::DefaultArgument<double, 1>(0.42));

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[default_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[default_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


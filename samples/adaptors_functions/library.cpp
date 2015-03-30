//
//  library.cpp
//  integral
//
//  Copyright (C) 2015  André Pereira Henriques
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

#include <iostream>
#include <functional>
#include <string>
#include <lua.hpp>
#include "integral.h"

class Object {
public:
    inline Object(const std::string &name) : name_(name) {}
    std::string getName() const {
        return name_;
    }
private:
    const std::string name_;
};

extern "C" {
    LUALIB_API int luaopen_liblibrary(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            integral::pushClassMetatable<Object>(luaState);
            integral::setFunction(luaState, "getName", &Object::getName);
            lua_pop(luaState, 1);

            using ObjectConstructorAdaptorType = integral::CConstructor<Object, const std::string &>;
            integral::setFunction(luaState, "getObjectConstructor", std::function<ObjectConstructorAdaptorType()>([]() -> ObjectConstructorAdaptorType {
                return ObjectConstructorAdaptorType();
            }));

            lua_pushstring(luaState, "getInverse");
            integral::push<integral::CFunction<double(double)>>(luaState, [](double x) -> double {
                return 1.0/x;
            });
            lua_rawset(luaState, -3);

            integral::setFunction(luaState, "getConstantFunction", std::function<integral::CLuaFunction(double)>([](double constant) -> integral::CLuaFunction {
                return [constant](lua_State *luaState) -> int {
                    integral::push<double>(luaState, constant);
                    return 1;
                };
            }));

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[adaptors_functions sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[adaptors_functions sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

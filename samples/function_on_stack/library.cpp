//
//  library.cpp
//  integral
//
//  Copyright (C) 2014, 2015  André Pereira Henriques
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
#include "integral.hpp"

class Object {
public:
    inline Object(const std::string name) : name_(name) {}
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

            integral::setLuaFunction(luaState, "getPrefixFunction", [](lua_State *luaState) -> int {
                integral::pushLuaFunction(luaState,[](lua_State *luaState) -> int {
                    integral::push<std::string>(luaState, integral::get<std::string>(luaState, integral::LuaFunctionWrapper::getUpValueIndex(1))
                                                          + integral::get<std::string>(luaState, 1));
                    return 1;
                }, 1);
                return 1;
            });

            integral::setLuaFunction(luaState, "getSuffixFunction1", [](lua_State *luaState) -> int {
                std::string suffix = integral::get<std::string>(luaState, 1);
                integral::pushFunction(luaState, [suffix](const std::string &string) -> std::string {
                    return string + suffix;
                });
                return 1;
            });

            // using function adaptor "integral::CFunction<T>":
            using SuffixFunctionType = integral::CFunction<std::string(const std::string &)>;
            integral::setFunction(luaState, "getSuffixFunction2", [](const std::string &suffix) -> SuffixFunctionType {
                return [suffix](const std::string &string) -> std::string {
                    return string + suffix;
                };
            });

            integral::pushClassMetatable<Object>(luaState);
            integral::setFunction(luaState, "__tostring", &Object::getName);
            lua_pop(luaState, 1);

            integral::push<std::string>(luaState, "newObject");
            integral::pushConstructor<Object, std::string>(luaState);
            lua_rawset(luaState, -3);
            // equivalent to:
            //integral::setConstructor<Object, std::string>(luaState, "newObject");

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[function_on_stack sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[function_on_stack sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

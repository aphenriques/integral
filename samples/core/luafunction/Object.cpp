//
//  Object.cpp
//  integral
//
//  Copyright (C) 2014, 2016, 2017, 2019  André Pereira Henriques
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
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>
#include <integral/utility.hpp>

class Object {
public:
    std::string string_;

    Object(const std::string &string) : string_(string) {}
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);
            integral::setConstructor<Object(std::string)>(luaState, "new");
            integral::setGetter(luaState, "getString", &Object::string_);

            // lua function
            integral::setLuaFunction(luaState, "createSuffixedObjectLuaFunction", [](lua_State *lambdaLuaState) -> int {
                    // Exceptions can be thrown normally from this function (integral manages it)
                    Object &object = integral::get<Object>(lambdaLuaState, 1);
                    std::string suffix = integral::get<std::string>(lambdaLuaState, 2);
                    // lua api works as usual, e.g:
                    // std::string suffix(lua_checkstring(lambdaLuaState, 2));
                    // Though it is not recommended because, in case of an error, destructors of the objects inside this function scope will not be called (lua_error performs long jump)
                    // integral::push and integral::get are safer
                    integral::push<Object>(lambdaLuaState, object.string_ + suffix);
                return 1;
            });
            //
            // lua function
            integral::setLuaFunction(luaState, "addSuffixLuaFunction", [](lua_State *lambdaLuaState) -> int {
                    // get by reference
                    Object &object = integral::get<Object>(lambdaLuaState, 1);
                    std::string suffix = integral::get<std::string>(lambdaLuaState, 2);
                    object.string_ += suffix;
                return 0;
            });

            // LuaFunctions accepts upvalues. Though their indices are different than Lua API (as follows)
            integral::push<std::string>(luaState, "upvalue!");
            integral::setLuaFunction(luaState, "printUpvalue", [](lua_State *lambdaLuaState) -> int {
                // upvalue index is offset by 1 (because of integral internals). integral::LuaFunctionWrapper::getUpValueIndex or lua_upvalueindex(index + 1) should be used to index upvalues
                std::cout << integral::get<std::string>(lambdaLuaState, integral::LuaFunctionWrapper::getUpValueIndex(1)) << std::endl;
                return 0;
            }, 1); // 1 upvalue

            // LuaFunctions can be pushed onto the stack
            integral::push<std::string>(luaState, "getPrinter");
            integral::pushLuaFunction(luaState, [](lua_State *lambdaLuaState) -> int {
                // LuaFunctions can be pushed onto the stack to be returned
                // lambda captures can be used as well as upvalues
                std::string printerName = integral::get<std::string>(lambdaLuaState, 1);
                integral::push<std::string>(lambdaLuaState, "upvalue_message!");
                integral::pushLuaFunction(lambdaLuaState, [printerName](lua_State *lambdaLambdaLuaState) -> int {
                    std::cout << printerName << ": " << integral::get<std::string>(lambdaLambdaLuaState, integral::LuaFunctionWrapper::getUpValueIndex(1)) << std::endl;
                    return 0;
                }, 1);
                return 1;
            });
            lua_rawset(luaState, -3);

            // alternative way to push LuaFunctions
            lua_pushstring(luaState, "printMessage");
            integral::push<integral::LuaFunctionWrapper>(luaState, [](lua_State * /*lambdaLuaState*/) -> int {
                std::cout << "message from LuaFunctionWrapper" << std::endl;
                return 0;
            });
            lua_rawset(luaState, -3);

            integral::utility::printStack(luaState);
            integral::setLuaFunction(luaState, "printStack", integral::utility::printStack);
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[luafunction sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[luafunction sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

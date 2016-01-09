//
//  LuaFunctionWrapper.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016  André Pereira Henriques
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

#include "LuaFunctionWrapper.h"
#include <exception>
#include <string>
#include <lua.hpp>
#include "exception/Exception.h"
#include "basic.h"
#include "lua_compatibility.h"
#include "message.h"

namespace integral {
    namespace detail {
        const char * const LuaFunctionWrapper::kMetatableName_ = "integral_LuaFunctionWrapperMetatableName";
        
        void LuaFunctionWrapper::pushFunction(lua_State *luaState, const LuaFunctionWrapper &luaFunction, int nUpValues) {
            basic::pushUserData<LuaFunctionWrapper>(luaState, luaFunction);
            basic::pushClassMetatable<LuaFunctionWrapper>(luaState, kMetatableName_);
            lua_setmetatable(luaState, -2);
            if (nUpValues != 0) {
                lua_insert(luaState, -1 - nUpValues);
            }
            lua_pushcclosure(luaState, [](lua_State *luaState) {
                try {
                    LuaFunctionWrapper *luaFunctionWrapper = static_cast<LuaFunctionWrapper *>(lua_compatibility::testudata(luaState, lua_upvalueindex(1), kMetatableName_));
                    if (luaFunctionWrapper != nullptr) {
                        return luaFunctionWrapper->call(luaState);
                    } else {
                        throw exception::LogicException(__FILE__, __LINE__, __func__, "corrupted LuaFunctionWrapper");
                    }
                } catch (const std::exception &exception) {
                    lua_pushstring(luaState, (std::string("[integral] ") + exception.what()).c_str());
                } catch (...) {
                    lua_pushstring(luaState, message::gkUnknownExceptionMessage);
                }
                // error return outside catch scope so that the exception destructor can be called
                return lua_error(luaState);
            }, 1 + nUpValues);
        }
        
        void LuaFunctionWrapper::setFunction(lua_State *luaState, const std::string &name, const LuaFunctionWrapper &luaFunction, int nUpValues) {
            if (lua_istable(luaState, -1 - nUpValues) != 0) {
                pushFunction(luaState, luaFunction, nUpValues);
                lua_pushstring(luaState, name.c_str());
                lua_insert(luaState, -2);
                lua_rawset(luaState, -3);
            } else {
                throw exception::LogicException(__FILE__, __LINE__, __func__, message::gkInvalidStackExceptionMessage);
            }
        }
    }
}

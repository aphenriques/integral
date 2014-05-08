//
//  LuaFunctionWrapper.cpp
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

#include "LuaFunctionWrapper.h"
#include <string>
#include <exception>
#include <stdexcept>
#include <lua.hpp>
#include "basic.h"

namespace integral {
    namespace detail {
        const char * const LuaFunctionWrapper::kMetatableName_ = "integral_LuaFunctionWrapperMetatableName";
        
        void LuaFunctionWrapper::setFunction(lua_State *luaState, const std::string &name, const LuaFunctionWrapper &luaFunction) {
            basic::pushUserData<LuaFunctionWrapper>(luaState, luaFunction);
            basic::pushClassMetatable<LuaFunctionWrapper>(luaState, kMetatableName_);
            lua_setmetatable(luaState, -2);
            basic::setLuaFunction(luaState, name, [](lua_State *luaState) {
                try {
                    LuaFunctionWrapper *luaFunctionWrapper = static_cast<LuaFunctionWrapper *>(luaL_testudata(luaState,lua_upvalueindex(1), kMetatableName_));
                    if (luaFunctionWrapper != nullptr) {
                        return luaFunctionWrapper->call(luaState);
                    } else {
                        throw std::runtime_error("corrupted LuaFunctionWrapper");
                    }
                } catch (const std::exception &exception) {
                    lua_pushstring(luaState, (std::string("[integral] ") + exception.what()).c_str());
                } catch (...) {
                    lua_pushstring(luaState, basic::gkUnknownExceptionMessage);
                }
                // error return outside catch scope so that the exception destructor can be called
                return lua_error(luaState);
            }, 1);
        }
    }
}

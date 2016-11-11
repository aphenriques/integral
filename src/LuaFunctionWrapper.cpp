//
//  LuaFunctionWrapper.cpp
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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

#include "LuaFunctionWrapper.hpp"

namespace integral {
    namespace detail {
        namespace exchanger {
            const char * const Exchanger<LuaFunctionWrapper>::kMetatableName_ = "integral_LuaFunctionWrapperMetatableName";

            LuaFunctionWrapper Exchanger<LuaFunctionWrapper>::get(lua_State *luaState, int index) {
                if (lua_iscfunction(luaState, index) == 0) {
                    if (lua_getupvalue(luaState, index, 1) != nullptr) {
                        // stack: upvalue
                        const LuaFunctionWrapper *luaFunctionWrapperPointer = static_cast<LuaFunctionWrapper *>(lua_compatibility::testudata(luaState, -1, kMetatableName_));
                        if (luaFunctionWrapperPointer != nullptr) {
                            LuaFunctionWrapper luaFunctionWrapper = *luaFunctionWrapperPointer;
                            lua_pop(luaState, 1);
                            // stack:
                            return std::move(luaFunctionWrapper);
                        } else {
                            const ArgumentException argumentException = ArgumentException::createTypeErrorException(luaState, index, "LuaFunctionWrapper");
                            lua_pop(luaState, 1);
                            // stack:
                            throw argumentException;
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "LuaFunctionWrapper (no upvalue)");
                    }
                } else {
                    throw ArgumentException::createTypeErrorException(luaState, index, "lua_CFuntion");
                }
            }
        }
    }
}

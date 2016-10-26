//
//  LuaFunctionArgument.cpp
//  integral
//
//  Copyright (C) 2014, 2016  André Pereira Henriques
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

#include "LuaFunctionArgument.hpp"
#include <lua.hpp>
#include "Adaptor.hpp"
#include "ArgumentException.hpp"
#include "lua_compatibility.hpp"

namespace integral {
    namespace detail {
        LuaFunctionArgument::LuaFunctionArgument(lua_State *luaState, int index) : luaState_(luaState), luaAbsoluteStackIndex_(lua_compatibility::absindex(luaState, index)) {}

        namespace exchanger {
            LuaFunctionArgument Exchanger<LuaFunctionArgument>::get(lua_State *luaState, int index) {
                if (lua_isfunction(luaState, index) != 0) {
                    // Adaptor<LuaFunctionArgument> is utilized to access protected constructor of LuaFunctionArgument
                    return Adaptor<LuaFunctionArgument>(luaState, index);
                } else {
                    throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TFUNCTION));
                }
            }
        }
    }
}

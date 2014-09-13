//
//  lua_compatibility.cpp
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
#include "lua_compatibility.h"
#include <lua.hpp>

namespace integral {
    namespace detail {
        namespace lua_compatibility {
            
#if LUA_VERSION_NUM == 501
            void * testudata(lua_State *luaState, int index, const char *metatableName) {
                void *p = lua_touserdata(luaState, index);
                if (p != NULL) {  // value is a userdata?
                    if (lua_getmetatable(luaState, index)) {  // does it have a metatable?
                        luaL_getmetatable(luaState, metatableName);  // get correct metatable
                        if (!lua_rawequal(luaState, -1, -2))  // not the same?
                            p = NULL;  // value is a userdata with wrong metatable
                        lua_pop(luaState, 2);  // remove both metatables
                        return p;
                    }
                }
                return NULL;  // value is not a userdata with a metatable
            }
#endif
            
#if LUA_VERSION_NUM == 501
            int absindex(lua_State *luaState, int index) {
                return index > 0 || index <= LUA_REGISTRYINDEX ? index : lua_gettop(luaState) + index + 1;
            }
#endif
            
#if LUA_VERSION_NUM == 501
            void copy(lua_State *luaState, int fromIndex, int toIndex) {
                // TODO test this
                lua_pushvalue(luaState, fromIndex);
                lua_replace(luaState, absindex(luaState, toIndex));
            }
#endif
            
#if LUA_VERSION_NUM == 501
            lua_Integer tointegerx(lua_State *luaState, int index, int *isNumber) {
                *isNumber = lua_isnumber(luaState, index);
                return lua_tointeger(luaState, index);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            unsigned long tounsignedx(lua_State *luaState, int index, int *isNumber) {
                *isNumber = lua_isnumber(luaState, index);
                return static_cast<unsigned long>(lua_tointeger(luaState, index));
            }
#endif
            
#if LUA_VERSION_NUM == 501
            lua_Number tonumberx(lua_State *luaState, int index, int *isNumber) {
                *isNumber = lua_isnumber(luaState, index);
                return lua_tonumber(luaState, index);
            }
#endif
        }
    }
}
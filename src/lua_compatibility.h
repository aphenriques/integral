//
//  lua_compatibility.h
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

#ifndef integral_lua_compatibility_h
#define integral_lua_compatibility_h

#include <lua.hpp>

namespace integral {
    namespace detail {
        namespace lua_compatibility {
            
#if LUA_VERSION_NUM == 501
            constexpr int keLuaOk = 0;
#else
            constexpr int keLuaOk = LUA_OK;
#endif
            
#if LUA_VERSION_NUM == 501
            void * testudata(lua_State *luaState, int index, const char *metatableName);
#else
            inline void * testudata(lua_State *luaState, int index, const char *metatableName) {
                return luaL_testudata(luaState, index, metatableName);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            inline size_t rawlen(lua_State *luaState, int index) {
                return lua_objlen(luaState, index);
            }
#else
            inline size_t rawlen(lua_State *luaState, int index) {
                return lua_rawlen(luaState, index);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            inline void pushglobaltable(lua_State *luaState) {
                lua_pushvalue(luaState, LUA_GLOBALSINDEX);
            }
#else
            inline void pushglobaltable(lua_State *luaState) {
                lua_pushglobaltable(luaState);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            int absindex(lua_State *luaState, int index);
#else
            inline int absindex(lua_State *luaState, int index) {
                return lua_absindex(luaState, index);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            void copy(lua_State *luaState, int fromIndex, int toIndex);
#else
            inline void copy(lua_State *luaState, int fromIndex, int toIndex) {
                lua_copy(luaState, fromIndex, toIndex);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            template<typename T>
            inline void pushunsigned(lua_State *luaState, T number) {
                lua_pushinteger(luaState, static_cast<lua_Integer>(number));
            }
#elif LUA_VERSION_NUM == 502
            template<typename T>
            inline void pushunsigned(lua_State *luaState, T number) {
                lua_pushunsigned(luaState, static_cast<lua_Unsigned>(number));
            }
#else
            template<typename T>
            inline void pushunsigned(lua_State *luaState, T number) {
                lua_pushinteger(luaState, static_cast<lua_Unsigned>(number));
            }
#endif
            
#if LUA_VERSION_NUM == 501
            lua_Integer tointegerx(lua_State *luaState, int index, int *isNumber);
#else
            inline lua_Integer tointegerx(lua_State *luaState, int index, int *isNumber) {
                return lua_tointegerx(luaState, index, isNumber);
            }
#endif
            
#if LUA_VERSION_NUM == 501
            unsigned long tounsignedx(lua_State *luaState, int index, int *isNumber);
#elif LUA_VERSION_NUM == 502
            inline lua_Unsigned tounsignedx(lua_State *luaState, int index, int *isNumber) {
                return lua_tounsignedx(luaState, index, isNumber);
            }
#else
            inline lua_Unsigned tounsignedx(lua_State *luaState, int index, int *isNumber) {
                return static_cast<lua_Unsigned>(lua_tointegerx(luaState, index, isNumber));
            }
#endif
            
#if LUA_VERSION_NUM == 501
            lua_Number tonumberx(lua_State *luaState, int index, int *isNumber);
#else
            inline lua_Number tonumberx(lua_State *luaState, int index, int *isNumber) {
                return lua_tonumberx(luaState, index, isNumber);
            }
#endif
            
        }
    }
}

#endif

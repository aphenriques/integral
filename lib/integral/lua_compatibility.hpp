//
//  lua_compatibility.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef integral_lua_compatibility_hpp
#define integral_lua_compatibility_hpp

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
                lua_pushinteger(luaState, static_cast<lua_Integer>(number));
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

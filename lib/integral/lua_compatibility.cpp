//
//  lua_compatibility.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include "lua_compatibility.hpp"
#include <lua.hpp>

namespace integral {
    namespace detail {
        namespace lua_compatibility {

#if LUA_VERSION_NUM == 501
            void * testudata(lua_State *luaState, int index, const char *metatableName) {
                void *userData = lua_touserdata(luaState, index);
                if (userData != nullptr) {
                    if (lua_getmetatable(luaState, index) != 0) {
                        luaL_getmetatable(luaState, metatableName);
                        if (lua_rawequal(luaState, -1, -2) == 0) {
                            userData = nullptr;
                        }
                        lua_pop(luaState, 2);
                        return userData;
                    }
                }
                return nullptr;
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

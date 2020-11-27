//
//  basic.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_basic_hpp
#define integral_basic_hpp

#include <string>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "lua_compatibility.hpp"

namespace integral {
    namespace detail {
        namespace basic {
            void setLuaFunction(lua_State *luaState, const char *name, lua_CFunction function, int nUpValues);

            inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function, int nUpValues);

            template<typename T, typename ...A>
            inline void pushUserData(lua_State *luaState, A &&...arguments);

            template<typename T>
            bool pushClassMetatable(lua_State *luaState, const char *name);

            template<typename T>
            inline bool pushClassMetatable(lua_State *luaState, const std::string &name);

            template<typename T>
            void pushClassMetatable(lua_State *luaState);

            //--

            inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function, int nUpValues) {
                setLuaFunction(luaState, name.c_str(), function, nUpValues);
            }

            template<typename T, typename ...A>
            inline void pushUserData(lua_State *luaState, A &&...arguments) {
                new(lua_compatibility::newuserdata(luaState, sizeof(T))) T(std::forward<A>(arguments)...);
            }

            template<typename T>
            bool pushClassMetatable(lua_State *luaState, const char *name) {
                if (luaL_newmetatable(luaState, name) != 0) {
                    // metatable.__index = metatable
                    lua_pushstring(luaState, "__index");
                    lua_pushvalue(luaState, -2); // duplicates the metatable
                    lua_rawset(luaState, -3);
                    setLuaFunction(luaState, "__gc", [](lua_State *lambdaLuaState) -> int {
                        static_cast<T *>(lua_touserdata(lambdaLuaState, 1))->~T();
                        return 0;
                    }, 0);
                    return true;
                }
                return false;
            }

            template<typename T>
            inline bool pushClassMetatable(lua_State *luaState, const std::string &name) {
                return pushClassMetatable<T>(luaState, name.c_str());
            }

            template<typename T>
            void pushClassMetatable(lua_State *luaState) {
                lua_newtable(luaState);
                // metatable.__index = metatable
                lua_pushstring(luaState, "__index");
                lua_pushvalue(luaState, -2); // duplicates the metatable
                lua_rawset(luaState, -3);
                setLuaFunction(luaState, "__gc", [](lua_State *lambdaLuaState) -> int {
                    static_cast<T *>(lua_touserdata(lambdaLuaState, 1))->~T();
                    return 0;
                }, 0);
            }
        }
    }
}

#endif

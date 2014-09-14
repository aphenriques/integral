//
//  basic.h
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

#ifndef integral_basic_h
#define integral_basic_h

#include <string>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "IsStringLiteral.h"

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
                new(lua_newuserdata(luaState, sizeof(T))) T(std::forward<A>(arguments)...);
            }
            
            template<typename T>
            bool pushClassMetatable(lua_State *luaState, const char *name) {
                if (luaL_newmetatable(luaState, name) != 0) {
                    // metatable.__index = metatable
                    lua_pushstring(luaState, "__index");
                    lua_pushvalue(luaState, -2); // duplicates the metatable
                    lua_rawset(luaState, -3);
                    setLuaFunction(luaState, "__gc", [](lua_State *luaState) {
                        static_cast<T *>(lua_touserdata(luaState, 1))->~T();
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
                setLuaFunction(luaState, "__gc", [](lua_State *luaState) {
                    static_cast<T *>(lua_touserdata(luaState, 1))->~T();
                    return 0;
                }, 0);
            }
        }
    }
}

#endif

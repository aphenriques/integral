//
//  exchanger.cpp
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

#include "exchanger.h"

namespace integral {
    namespace detail {
        namespace exchanger {
            const char * Exchanger<const char *>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    const char * const string = lua_tostring(luaState, index);
                    if (string == nullptr) {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                    return string;
                } else {
                    const char **userDataBase = type_manager::getConvertibleType<const char *>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                }
            }
            
            std::string Exchanger<std::string>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    const char * const string = lua_tostring(luaState, index);
                    if (string == nullptr) {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                    return string;
                } else {
                    return getObject<std::string>(luaState, index);
                }
            }
            
            
            void Exchanger<std::string>::push(lua_State *luaState, const std::string &string) {
                if (type_manager::checkClassMetatableExistence<std::string>(luaState) == false) {
                    lua_pushstring(luaState, string.c_str());
                } else {
                    pushObject<std::string>(luaState, string);
                }
            }
            
            bool Exchanger<bool>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    return lua_toboolean(luaState, index);
                } else {
                    bool *userDataBase = type_manager::getConvertibleType<bool>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TBOOLEAN));
                    }
                }
            }
        }
    }
}

//
//  exchanger.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016  André Pereira Henriques
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
                    if (string != nullptr) {
                        return string;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                } else {
                    const char **userData = type_manager::getConvertibleType<const char *>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                }
            }
            
            std::string Exchanger<std::string>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    const char * const string = lua_tostring(luaState, index);
                    if (string != nullptr) {
                        return string;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                } else {
                    std::string *userData = type_manager::getConvertibleType<std::string>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TSTRING));
                    }
                }
            }
            
            bool Exchanger<bool>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    return lua_toboolean(luaState, index);
                } else {
                    bool *userData = type_manager::getConvertibleType<bool>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TBOOLEAN));
                    }
                }
            }
        }
    }
}

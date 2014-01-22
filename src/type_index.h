//
//  type_index.h
//  integral
//
//  Copyright (C) 2013  André Pereira Henriques
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

#ifndef integral_type_index_h
#define integral_type_index_h

#include <stdexcept>
#include <typeindex>
#include <lua.hpp>
#include "basic.h"
#include "UserDataWrapper.h"

namespace integral {
    namespace type_index {
        extern const char * const gkTypeIndexMetatableName;
        
        extern const char * const gkTypeFunctionsKey;
        
        void pushTypeIndexUserData(lua_State *luaState, const std::type_index &typeIndex);

        template<typename T>
        inline void pushTypeIndexUserData(lua_State *luaState);
        
        template<typename D, typename B>
        void setTypeFunctionFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex);
        
        template<typename D, typename B>
        void setTypeFunctionFromTypeTable(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash);
        
        template<typename D, typename B>
        void setTypeFunction(lua_State *luaState);
        
        template<typename T>
        T * getBaseType(lua_State *luaState, int index);
        
        //--
        
        template<typename T>
        inline void pushTypeIndexUserData(lua_State *luaState) {
            pushTypeIndexUserData(luaState, typeid(T));
        }

        template<typename D, typename B>
        void setTypeFunctionFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex) {
            // stack: metatable | typeHashTable
            type_index::pushTypeIndexUserData(luaState, typeIndex);
            // stack: metatable | typeHashTable| type_index_udata*
            lua_pushcclosure(luaState, [](lua_State *luaState) -> int {
                lua_pushlightuserdata(luaState, static_cast<B *>(static_cast<D *>(lua_touserdata(luaState, 1))));
                return 1;
            }, 0);
            // stack: metatable | typeHashTable | type_index_udata* | function*
            lua_rawset(luaState, -3);
            lua_pop(luaState, 1);
            // stack: metatable
        }

        
        template<typename D, typename B>
        void setTypeFunctionFromTypeTable(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash) {
            // stack: metatable | typeTable
            lua_pushinteger(luaState, typeHash);
            lua_newtable(luaState);
            lua_pushvalue(luaState, -1);
            // stack: metatable | typeTable | typeHash | typeHashTable* | typeHashTable*
            lua_insert(luaState, -4);
            // stack: metatable | typeHashTable* | typeTable | typeHash | typeHashTable*
            lua_rawset(luaState, -3);
            // stack: metatable | typeHashTable | typeTable
            lua_pop(luaState, 1);
            // stack: metatable | typeHashTable
            setTypeFunctionFromTypeHashTable<D, B>(luaState, typeIndex);
        }
        
        // metatable[gkTypeFunctionsKey] = {[typeHash] = {[type_index_udata] = function}}
        // metatable[gkTypeFunctionsKey] = {[typeHash] = typeHashTable}
        // metatable[gkTypeFunctionsKey] = typeTable
        template<typename D, typename B>
        void setTypeFunction(lua_State *luaState) {
            const std::type_index typeIndex = typeid(B);
            const size_t typeHash = typeIndex.hash_code();
            // stack: metatable
            lua_pushstring(luaState, gkTypeFunctionsKey);
            lua_rawget(luaState, -2);
            if (lua_istable(luaState, -1) != 0) {
                // stack: metatable | typeTable
                lua_pushinteger(luaState, typeHash);
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: metatable | typeTable | typeHashTable
                    lua_remove(luaState, -2);
                    // stack: metatable | typeHashTable
                    setTypeFunctionFromTypeHashTable<D, B>(luaState, typeIndex);
                    // stack: metatable
                } else {
                    // stack:  metatable | typeTable | nil
                    lua_pop(luaState, 1);
                    setTypeFunctionFromTypeTable<D, B>(luaState, typeIndex, typeHash);
                    // stack: metatable
                }
            } else {
                // stack: metatable | nil
                lua_pop(luaState, 1);
                lua_newtable(luaState);
                lua_pushstring(luaState, gkTypeFunctionsKey);
                // stack: metatable | typesTable* | gkTypeFunctionsKey
                lua_pushvalue(luaState, -2);
                // stack: metatable | typesTable* | gkTypeFunctionsKey | typesTable*
                lua_rawset(luaState, -4);
                // stack: metatable | typeTable
                setTypeFunctionFromTypeTable<D, B>(luaState, typeIndex, typeHash);
                // stack: metatable
            }
        }
        
        template<typename T>
        T * getBaseType(lua_State *luaState, int index) {
            const std::type_index typeIndex = typeid(T);
            const size_t typeHash = typeIndex.hash_code();
            lua_pushvalue(luaState, index);
            // stack: userdata
            if (lua_getmetatable(luaState, -1) != 0) {
                // stack: userdata | metatable
                lua_pushstring(luaState, gkTypeFunctionsKey);
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: userdata | metatable | typeTable
                    lua_pushinteger(luaState, typeHash);
                    lua_rawget(luaState, -2);
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: userdata | metatable | typeTable | typeHashTable
                        lua_remove(luaState, -2);
                        // stack: userdata | metatable | typeHashTable
                        lua_pushnil(luaState);
                        // stack: userdata | metatable | typeHashTable | nil
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            // stack: userdata | metatable | typeHashTable | type_index_udata | function
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -2, type_index::gkTypeIndexMetatableName)); // FIXME remove metatable checking?
                            if (storedTypeIndex != nullptr) {
                                if (*storedTypeIndex == typeIndex) {
                                    lua_remove(luaState, -2);
                                    lua_remove(luaState, -2);
                                    lua_remove(luaState, -2);
                                    // stack: userdata | function
                                    if (lua_iscfunction(luaState, -1) != 0) {
                                        lua_pushvalue(luaState, -2);
                                        // stack: userdata | function | userdata
                                        lua_call(luaState, 1, 1);
                                        // stack: userdata | lighuserdata
                                        if (lua_islightuserdata(luaState, -1) != 0) {
                                            T * userData = static_cast<T *>(lua_touserdata(luaState, -1));
                                            lua_pop(luaState, 2);
                                            return userData;
                                        } else {
                                            lua_pop(luaState, 2);
                                            throw std::runtime_error("corrupted TypeFunction typeHashTable - 1");
                                        }
                                    } else {
                                        lua_pop(luaState, 2);
                                        throw std::runtime_error("corrupted TypeFunction typeHashTable - 2");
                                    }
                                }
                            } else {
                                // stack: userdata | metatable | typeHashTable | ? | function?
                                lua_pop(luaState, 5);
                                throw std::runtime_error("corrupted TypeFunction typeHashTable - 3");
                            }
                            // stack: userdata | metatable | typeHashTable | type_index_udata | function
                        }
                        // stack: userdata | metatable | typeHashTable
                        lua_pop(luaState, 3);
                    } else {
                        // stack: userdata | metatable | typeTable | nil
                        lua_pop(luaState, 4);
                    }
                } else {
                    // stack: userdata | metatable | nil
                    lua_pop(luaState, 3);
                }
            } else {
                // stack: userdata
                lua_pop(luaState, 1);
            }
            return nullptr;
        }
    }
}

#endif

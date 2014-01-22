//
//  type_manager.h
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

#ifndef integral_type_manager_h
#define integral_type_manager_h

#include <stdexcept>
#include <functional>
#include <typeindex>
#include <lua.hpp>
#include "basic.h"
#include "type_index.h"
#include "UserDataWrapper.h"

// TypeManger is implemented in lua as a solution so that it is not needed that a class metatable name is provided in c++ code.
// typeid(T).name() cannot be used as an automatic indentifier for metatables. It is not safe: different classes might have the same typeid(T).name() (c++ standard)
// std::type_index is used as an automatic indentifier for metatables. Supposedly, it is safe.

// TypeManager organization (hashmap):
// REGISTRY[gkTypeMangerRegistryKey] = {[typeHash] = {[type_index_udata] = rootMetatable}}
// REGISTRY[gkTypeMangerRegistryKey] = {[typeHash] = typeHashTable}
// REGISTRY[gkTypeMangerRegistryKey] = typeManager

// TypeManager (integral) version control:
// REGISTRY[gkTypeMangerRegistryKey][gkVersionKey] = gkVersion

// there might be clashes with typeHash. It is NOT a problem. TypeManager is implemented as a hashmap.

// Attention! The stored type_index (type_index_udata) is UserDataWrapper<T>
// This is useful when multiple integral versions are used.
// Maybe UserDataWrapper<T> is incompatible from different integral versions used together; this way, it will fail gracefully.

namespace integral {
    namespace type_manager {
        extern const char * const gkTypeMangerRegistryKey;
        
        extern const char * const gkTypeIndexKey;
        
        extern const char * const gkVersionKey;
        
        constexpr unsigned gkVersion = 1;
        
        template<typename T>
        UserDataWrapper<T> * getUserDataWrapper(lua_State *luaState, int index);
        
        template<typename T>
        void pushRootMetatableFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex);
        
        template<typename T>
        void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash);
        
        template<typename T>
        bool pushClassMetatable(lua_State *luaState);
        
        //--
        
        template<typename T>
        UserDataWrapper<T> * getUserDataWrapper(lua_State *luaState, int index) {
            void * userData = lua_touserdata(luaState, index);
            if (userData != nullptr) {
                if (lua_getmetatable(luaState, index) != 0) {
                    //stack: metatable
                    lua_pushstring(luaState, gkTypeIndexKey);
                    //stack: metatable | gkTypeIndexKey
                    lua_rawget(luaState, -2);
                    if (lua_isuserdata(luaState, -1) != 0) {
                        //stack: metatable | type_index
                        std::type_index *userDataTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -1, type_index::gkTypeIndexMetatableName));
                        if (userDataTypeIndex == nullptr) {
                            throw std::runtime_error("corrupted userdata type_index");
                        }
                        if (*userDataTypeIndex != std::type_index(typeid(UserDataWrapper<T>))) {
                            userData = nullptr;
                        }
                    } else {
                        //stack: metatable | nil
                        userData = nullptr;
                    }
                    lua_pop(luaState, 2);
                } else {
                    userData = nullptr;
                }
            }
            return static_cast<UserDataWrapper<T> *>(userData);
        }

        template<typename T>
        void pushRootMetatableFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex) {
            // stack: typeHashTable
            type_index::pushTypeIndexUserData(luaState, typeIndex);
            // stack: typeHashTable | type_index_udata*
            lua_newtable(luaState);
            // stack: typeHashTable | type_index_udata* | rootMetatable*
            lua_pushstring(luaState, "__index");
            lua_pushvalue(luaState, -2); // duplicates the metatable
            lua_rawset(luaState, -3); // metatable.__index = metatable
            basic::setLuaFunction(luaState, "__gc", [](lua_State *luaState) -> int {
                static_cast<UserDataWrapper<T> *>(lua_touserdata(luaState, 1))->~UserDataWrapper<T>();
                return 0;
            }, 0);
            // stack: typeHashTable | type_index_udata* | rootMetatable*
            type_index::setTypeFunction<UserDataWrapper<T>, UserDataWrapperBase>(luaState); // Sets UserDataWrapperBase type function
            // stack: typeHashTable | type_index_udata* | rootMetatable*
            lua_pushstring(luaState, gkTypeIndexKey);
            // stack: typeHashTable | type_index_udata* | rootMetatable* | gkTypeIndexKey
            lua_pushvalue(luaState, -3);
            // stack: typeHashTable | type_index_udata* | rootMetatable* | gkTypeIndexKey | type_index_udata*
            lua_rawset(luaState, -3); // used for getUserDataWrapper
            // stack: typeHashTable | type_index_udata* | rootMetatable*
            lua_pushvalue(luaState, -1);
            // stack: typeHashTable | type_index_udata* | rootMetatable* | rootMetatable*
            lua_insert(luaState, -4);
            // stack: rootMetatable* | typeHashTable | type_index_udata* | rootMetatable*
            lua_rawset(luaState, -3);
            lua_pop(luaState, 1);
            // stack: rootMetatable
        }
        
        template<typename T>
        void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash) {
            // stack: typeManager
            lua_pushinteger(luaState, typeHash);
            lua_newtable(luaState);
            lua_pushvalue(luaState, -1);
            // stack: typeManager | typeHash | typeHashTable* | typeHashTable*
            lua_insert(luaState, -4);
            // stack: typeHashTable* | typeManager | typeHash | typeHashTable*
            lua_rawset(luaState, -3);
            // stack: typeHashTable | typeManager
            lua_pop(luaState, 1);
            // stack: typeHashTable
            pushRootMetatableFromTypeHashTable<T>(luaState, typeIndex);
            // stack: rootMetatable
        }
        
        template<typename T>
        bool pushClassMetatable(lua_State *luaState) {
            // Attention! The stored type_index is UserDataWrapper<T>
            // This is useful when multiple integral versions are used.
            // Maybe UserDataWrapper<T> is incompatible from different integral versions used together; this way, it will fail gracefully.
            const std::type_index typeIndex = typeid(UserDataWrapper<T>);
            const size_t typeHash = typeIndex.hash_code();
            lua_pushstring(luaState, gkTypeMangerRegistryKey);
            lua_rawget(luaState, LUA_REGISTRYINDEX);
            if (lua_istable(luaState, -1) != 0) {
                // stack: typeManager
                lua_pushinteger(luaState, typeHash);
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: typeManager | typeHashTable
                    lua_remove(luaState, -2);
                    // stack: typeHashTable
                    lua_pushnil(luaState);
                    for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                        std::type_index *storedTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -2, type_index::gkTypeIndexMetatableName)); // FIXME remove metatable checking?
                        // stack: typeHashTable | type_index_udata | rootMetatable
                        if (storedTypeIndex != nullptr) {
                            if (*storedTypeIndex == typeIndex) {
                                lua_remove(luaState, -2);
                                lua_remove(luaState, -2);
                                // stack: rootMetatable
                                return false;
                            }
                        } else {
                            lua_pop(luaState, 3);
                            throw std::runtime_error("corrupted TypeManager");
                        }
                        // stack: typeHashTable | type_index_udata | rootMetatable
                    }
                    // stack: typeHashTable
                    pushRootMetatableFromTypeHashTable<T>(luaState, typeIndex);
                    // stack: rootMetatable
                } else {
                    // stack: typeManager | nil
                    lua_pop(luaState, 1);
                    pushRootMetatableFromTypeManager<T>(luaState, typeIndex, typeHash);
                    // stack: rootMetatable
                }
            } else {
                // stack: nil
                lua_pop(luaState, 1);
                lua_newtable(luaState);
                lua_pushstring(luaState, gkTypeMangerRegistryKey);
                // stack: typeManager* | gkTypeMangerRegistryKey
                lua_pushvalue(luaState, -2);
                // stack: typeManager* | gkTypeMangerRegistryKey | typeManager*
                lua_rawset(luaState, LUA_REGISTRYINDEX);
                // stack: typeManager
                lua_pushstring(luaState, gkVersionKey);
                // stack: typeManager | gkVersionKey
                lua_pushunsigned(luaState, gkVersion);
                // stack: typeManager | gkVersionKey | gkVersion
                lua_rawset(luaState, -3);
                // stack: typeManager
                pushRootMetatableFromTypeManager<T>(luaState, typeIndex, typeHash);
                // stack: rootMetatable
            }
            return true;
        }
    }
}

#endif

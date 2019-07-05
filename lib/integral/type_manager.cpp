//
//  type_manager.cpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016, 2019  André Pereira Henriques
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

#include "type_manager.hpp"
#include <cstddef>
#include <typeindex>
#include <lua.hpp>
#include "basic.hpp"
#include "lua_compatibility.hpp"
#include "UnexpectedStackException.hpp"

namespace integral {
    namespace detail {
        namespace type_manager {
            const char * const gkTypeManagerRegistryKey = "integral_TypeManagerRegistryKey";
            const char * const gkTypeIndexKey = "integral_TypeIndexKey";
            const char * const gkTypeManagerVersionKey = "integral_TypeManagerVersionKey";
            const char * const gkTypeManagerVersion = "0.1";
            const char * const gkTypeIndexMetatableName = "integral_TypeIndexMetatableName";
            const char * const gkTypeFunctionsKey = "integral_TypeFunctionsKey";
            const char * const gkUserDataWrapperBaseTableKey = "integral_UserDataWrapperBaseTableKey";
            const char * const gkUnderlyingTypeFunctionKey = "integral_UnderlyingTypeFunctionKey";
            const char * const gkInheritanceSearchTagKey = "integral_InheritanceSearchTagKey";
            const char * const gkInheritanceKey = "integral_InheritanceKey";
            const char * const gkInheritanceIndexMetamethodKey = "integral_InheritanceIndexMetamethodKey";

            const std::type_index * getClassMetatableType(lua_State *luaState) {
                //stack: metatable
                lua_pushstring(luaState, gkTypeIndexKey);
                //stack: metatable | gkTypeIndexKey
                lua_rawget(luaState, -2);
                //stack: metatable | type_index (?)
                if (lua_isuserdata(luaState, -1) != 0) {
                    //stack: metatable | type_index (?)
                    const std::type_index *userDataTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -1, gkTypeIndexMetatableName));
                    if (userDataTypeIndex != nullptr) {
                        //stack: metatable | type_index
                        lua_pop(luaState, 1);
                        //stack: metatable
                        return userDataTypeIndex;
                    } else {
                        //stack: metatable | ?
                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted userdata type_index");
                    }
                } else {
                    //stack: metatable | ?
                    lua_pop(luaState, 1);
                    return nullptr;
                }
            }

            bool checkClassMetatableType(lua_State *luaState, const std::type_index &typeIndex) {
                //stack: metatable
                const std::type_index * metatableTypeIndex = getClassMetatableType(luaState);
                if (metatableTypeIndex != nullptr) {
                    if (*metatableTypeIndex == typeIndex) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            bool checkClassMetatableExistence(lua_State *luaState, const std::type_index &typeIndex) {
                lua_pushstring(luaState, gkTypeManagerRegistryKey);
                //stack: gkTypeManagerRegistryKey
                lua_rawget(luaState, LUA_REGISTRYINDEX);
                //stack: typeManager (?)
                if (lua_istable(luaState, -1) != 0) {
                    // stack: typeManager
                    static_assert(sizeof(decltype(typeIndex.hash_code())) <= sizeof(lua_Integer), "lua_Integer cannot accommodate typeIndex.hash_code()");
                    lua_pushinteger(luaState, static_cast<lua_Integer>(typeIndex.hash_code()));
                    // stack: typeManager | typeHash
                    lua_rawget(luaState, -2);
                    // stack: typeManager | typeHashBucket (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: typeManager | typeHashBucket
                        lua_remove(luaState, -2);
                        // stack: typeHashBucket
                        lua_pushnil(luaState);
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            // stack: typeHashBucket | type_index_udata (?) | rootMetatable (?)
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -2, gkTypeIndexMetatableName));
                            if (storedTypeIndex != nullptr) {
                                // stack: typeHashBucket | type_index_udata | rootMetatable (?)
                                if (*storedTypeIndex == typeIndex) {
                                    lua_pop(luaState, 3);
                                    return true;
                                }
                            } else {
                                // stack: typeHashBucket | ? | rootMetatable (?)
                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted userdata type_index in typeHashBucket in typeManager");
                            }
                            // stack: typeHashBucket | type_index_udata (?) | rootMetatable (?)
                        }
                        // stack: typeHashBucket
                        lua_pop(luaState, 1);
                        return false;
                    } else {
                        // stack: typeManager | nil (?)
                        lua_pop(luaState, 2);
                        return false;
                    }
                } else {
                    // stack: nil (?)
                    lua_pop(luaState, 1);
                    return false;
                }
            }

            void pushTypeIndexUserData(lua_State *luaState, const std::type_index &typeIndex) {
                basic::pushUserData<std::type_index>(luaState, typeIndex);
                basic::pushClassMetatable<std::type_index>(luaState, gkTypeIndexMetatableName);
                lua_setmetatable(luaState, -2);
            }

            void setTypeFunctionHashTable(lua_State *luaState, std::size_t baseTypeHash) {
                // stack: metatable | typeFunctionTable
                static_assert(sizeof(baseTypeHash) <= sizeof(lua_Integer), "lua_Integer cannot accommodate baseTypeHash");
                lua_pushinteger(luaState, static_cast<lua_Integer>(baseTypeHash));
                // stack: metatable | typeFunctionTable | typeHash
                lua_newtable(luaState);
                // stack: metatable | typeFunctionTable | typeHash | typeHashBucket*
                lua_pushvalue(luaState, -1);
                // stack: metatable | typeFunctionTable | typeHash | typeHashBucket* | typeHashBucket*
                lua_insert(luaState, -4);
                // stack: metatable | typeHashBucket* | typeFunctionTable | typeHash | typeHashBucket*
                lua_rawset(luaState, -3);
                // stack: metatable | typeHashBucket | typeFunctionTable
                lua_pop(luaState, 1);
                // stack: metatable | typeHashBucket
            }

            // this function should be used only for the creation of typeFunctionHashTable
            void pushTypeFunctionHashTable(lua_State *luaState, const std::type_index &baseTypeIndex) {
                // stack: metatable
                const std::size_t baseTypeHash = baseTypeIndex.hash_code();
                lua_pushstring(luaState, gkTypeFunctionsKey);
                // stack: metatable | gkTypeFunctionsKey
                lua_rawget(luaState, -2);
                // stack: metatable | typeFunctionTable (?)
                if (lua_istable(luaState, -1) != 0) {
                    // stack: metatable | typeFunctionTable
                    static_assert(sizeof(baseTypeHash) <= sizeof(lua_Integer), "lua_Integer cannot accommodate baseTypeHash");
                    lua_pushinteger(luaState, static_cast<lua_Integer>(baseTypeHash));
                    // stack: metatable | typeFunctionTable | typeHash
                    lua_rawget(luaState, -2);
                    // stack: metatable | typeFunctionTable | typeHashBucket (?)
                    if (lua_istable(luaState, -1) == 0) {
                        // stack:  metatable | typeFunctionTable | nil (?)
                        lua_pop(luaState, 1);
                        // stack:  metatable | typeFunctionTable
                        setTypeFunctionHashTable(luaState, baseTypeHash);
                        // stack: metatable | typeHashBucket
                    } else {
                        // stack: metatable | typeFunctionTable | typeHashBucket
                        lua_remove(luaState, -2);
                        // stack: metatable | typeHashBucket
                        lua_pushnil(luaState);
                        // stack: metatable | typeHashBucket | nil
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            // stack: metatable | typeHashBucket | type_index_udata (?) | function (?)
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -2, gkTypeIndexMetatableName));
                            // stack: metatable | typeHashBucket | type_index_udata (?) | function (?)
                            if (storedTypeIndex != nullptr) {
                                // stack: metatable | typeHashBucket | type_index_udata | function (?)
                                if (*storedTypeIndex == baseTypeIndex) {
                                    // stack: metatable | typeHashBucket | type_index_udata | function (?)
                                    throw exception::LogicException(__FILE__, __LINE__, __func__, "trying to set existing typeFunction");
                                }
                                // stack: metatable | typeHashBucket | type_index_udata | function (?)
                                // [*]
                            } else {
                                // stack: metatable | typeHashBucket | ? | function (?)
                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeFunction typeHashBucket: expected type_index_udata at index -2");
                            }
                            // [-]
                            // stack: metatable | typeHashBucket | type_index_udata | function (?)
                        }
                        // stack: metatable | typeHashBucket
                    }
                } else {
                    // stack: metatable | nil (?)
                    lua_pop(luaState, 1);
                    // stack: metatable
                    lua_newtable(luaState);
                    // stack: metatable | typeFunctionTable*
                    lua_pushstring(luaState, gkTypeFunctionsKey);
                    // stack: metatable | typeFunctionTable* | gkTypeFunctionsKey
                    lua_pushvalue(luaState, -2);
                    // stack: metatable | typeFunctionTable* | gkTypeFunctionsKey | typeFunctionTable*
                    lua_rawset(luaState, -4);
                    // stack: metatable | typeFunctionTable
                    setTypeFunctionHashTable(luaState, baseTypeHash);
                    // stack: metatable | typeHashBucket
                }
            }

            bool checkInheritanceTable(lua_State *luaState, const std::type_index &typeIndex) {
                // stack: inheritanceTable
                const std::size_t inheritanceTableLength = lua_compatibility::rawlen(luaState, -1);
                for (std::size_t i = 1; i <= inheritanceTableLength; ++i) {
                    // stack: inheritanceTable
                    lua_compatibility::pushunsigned(luaState, i);
                    // stack: inheritanceTable | i
                    lua_rawget(luaState, -2);
                    // stack: inheritanceTable | baseTable (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: inheritanceTable | baseTable
                        lua_rawgeti(luaState, -1, static_cast<lua_Integer>(InheritanceTable::kTypeIndexIndex));
                        // stack: inheritanceTable | baseTable | baseTypeIndex (?)
                        const std::type_index *baseTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -1, gkTypeIndexMetatableName));
                        // stack: inheritanceTable | baseTable | baseTypeIndex (?)
                        if (baseTypeIndex != nullptr) {
                            // stack: inheritanceTable | baseTable | baseTypeIndex
                            lua_pop(luaState, 2);
                            // stack: inheritanceTable
                            if (*baseTypeIndex == typeIndex) {
                                // stack: inheritanceTable
                                return true;
                            }
                            // stack: inheritanceTable
                            // [*]
                        } else {
                            // stack: inheritanceTable | baseTable | ?
                            throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: expected baseTypeIndex at index -1");
                        }
                    } else {
                        // stack: inheritanceTable | ?
                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: expected baseTable at index -1");
                    }
                    // [-]
                    // stack: inheritanceTable
                }
                // stack: inheritanceTable
                return false;
            }

            // not allowed to throw exception (this function is used in callInheritanceIndexMetamethod)
            // noexcept modifier is not suited because if lua is compiled with exceptions (c++) its functions can throw exceptions (this is not a problem)
            bool checkInheritanceSearchTag(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: metatable
                lua_pushstring(luaState, gkInheritanceSearchTagKey);
                // stack: metatable | gkInheritanceSearchTagKey
                lua_rawget(luaState, -2);
                // stack: metatable | nil or true
                if (lua_toboolean(luaState, -1) == 0) {
                    // stack: metatable | nil
                    lua_pop(luaState, 2);
                    // stack:
                    return false;
                } else {
                    // stack: metatable | true
                    lua_pop(luaState, 2);
                    // stack:
                    return true;
                }
            }

            // not allowed to throw exception (this function is used in callInheritanceIndexMetamethod)
            // noexcept modifier is not suited because if lua is compiled with exceptions (c++) its functions can throw exceptions (this is not a problem)
            void tagInheritanceSearch(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: metatable
                lua_pushstring(luaState, gkInheritanceSearchTagKey);
                // stack: metatable | gkInheritanceSearchTagKey
                lua_pushboolean(luaState, 1);
                // stack: metatable | gkInheritanceSearchTagKey | true
                lua_rawset(luaState, -3);
                // stack: metatable
                lua_pop(luaState, 1);
                // stack:
            }

            // not allowed to throw exception (this function is used in callInheritanceIndexMetamethod)
            // noexcept modifier is not suited because if lua is compiled with exceptions (c++) its functions can throw exceptions (this is not a problem)
            void untagInheritanceSearch(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: metatable
                lua_pushstring(luaState, gkInheritanceSearchTagKey);
                // stack: metatable | gkInheritanceSearchTagKey
                lua_pushnil(luaState);
                // stack: metatable | gkInheritanceSearchTagKey | nil
                lua_rawset(luaState, -3);
                // stack: metatable
                lua_pop(luaState, 1);
                // stack:
            }

            bool pushDirectConvertibleType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex) {
                // stack: metatable
                lua_pushvalue(luaState, index);
                // stack: metatable | [underlyingLight]UserData (?)
                lua_insert(luaState, -2);
                // stack: [underlyingLight]UserData (?) | metatable
                lua_pushstring(luaState, gkTypeFunctionsKey);
                // stack: [underlyingLight]UserData (?) | metatable | gkTypeFunctionsKey
                lua_rawget(luaState, -2);
                // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable (?)
                if (lua_istable(luaState, -1) != 0) {
                    // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable
                    static_assert(sizeof(decltype(convertibleTypeIndex.hash_code())) <= sizeof(lua_Integer), "lua_Integer cannot accommodate convertibleTypeIndex.hash_code()");
                    lua_pushinteger(luaState, static_cast<lua_Integer>(convertibleTypeIndex.hash_code()));
                    // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable | typeHash
                    lua_rawget(luaState, -2);
                    // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable | typeHashBucket (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable | typeHashBucket
                        lua_remove(luaState, -2);
                        // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket
                        lua_pushnil(luaState);
                        // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | nil
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | type_index_udata (?) | function (?)
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -2, gkTypeIndexMetatableName));
                            if (storedTypeIndex != nullptr) {
                                if (*storedTypeIndex == convertibleTypeIndex) {
                                    // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | type_index_udata | function (?)
                                    lua_remove(luaState, -2);
                                    // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | function (?)
                                    lua_remove(luaState, -2);
                                    // stack: [underlyingLight]UserData (?) | metatable | function (?)
                                    if (lua_iscfunction(luaState, -1) != 0) {
                                        // stack: [underlyingLight]UserData (?) | metatable | function
                                        if (lua_islightuserdata(luaState, -3) == 0) {
                                            // stack: userData (?) | metatable | function
                                            if (lua_isuserdata(luaState, -3) != 0) {
                                                // stack: userData | metatable | function
                                                lua_insert(luaState, -2);
                                                // stack: userdata | function | metatable
                                                if (pushUnderlyingTypeLightUserData(luaState, -3) == true) {
                                                    // stack: userdata | function | underlyingLightUserData
                                                    lua_remove(luaState, -3);
                                                    // stack: function | underlyingLightUserData
                                                    // [*]
                                                } else {
                                                    // stack: userdata | function
                                                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted UnderlyingTypeFunction");
                                                }
                                            } else {
                                                // stack: ? | metatable | function
                                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "failed to push direct convertible type: value at index -3 is neither a lightuserdata nor userdata");
                                            }
                                        } else {
                                            // stack: underlyingLightUserData | metatable | function
                                            lua_remove(luaState, -2);
                                            // stack: underlyingLightUserData | function
                                            lua_insert(luaState, -2);
                                            // stack: function | underlyingLightUserData
                                        }
                                        // [-]
                                        // stack: function | underlyingLightUserData
                                        lua_call(luaState, 1, 1);
                                        // stack: convertedLighuserdata (?)
                                        if (lua_islightuserdata(luaState, -1) != 0) {
                                            // stack: convertedLighuserdata
                                            return true;
                                        } else {
                                            // stack: ?
                                            throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeFunction typeHashBucket: expected convertedLightUserData at index -1");
                                        }
                                    } else {
                                        // stack: [underlyingLight]UserData (?) | metatable | ?
                                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeFunction typeHashBucket: expected cfunction at index -1");
                                    }
                                }
                            } else {
                                // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | ? | function (?)
                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeFunction typeHashBucket: expected type_index_udata at index -2");
                            }
                            // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket | type_index_udata | function (?)
                        }
                        // stack: [underlyingLight]UserData (?) | metatable | typeHashBucket
                        lua_pop(luaState, 3);
                    } else {
                        // stack: [underlyingLight]UserData (?) | metatable | typeFunctionTable | nil (?)
                        lua_pop(luaState, 4);
                    }
                } else {
                    // stack: [underlyingLight]UserData (?) | metatable | nil (?)
                    lua_pop(luaState, 3);
                }
                return false;
            }

            bool pushConvertibleOrInheritedType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isRecursion) {
                // stack: metatable
                if (isRecursion == false || checkInheritanceSearchTag(luaState, -1) == false) {
                    // stack: metatable
                    lua_pushvalue(luaState, index);
                    // stack: metatable | [underlyingLight]UserData (?)
                    lua_insert(luaState, -2);
                    // stack: [underlyingLight]UserData (?) | metatable
                    lua_pushvalue(luaState, -1);
                    // stack: [underlyingLight]UserData (?) | metatable | metatable
                    if (pushDirectConvertibleType(luaState, -3, convertibleTypeIndex) == true) {
                        // stack: underlyingLightUserData | metatable | convertedLightUserData
                        lua_remove(luaState, -2);
                        // stack: underlyingLightUserData | convertedLightUserData
                        lua_remove(luaState, -2);
                        // stack: convertedLightUserData
                        return true;
                    } else {
                        // stack: [underlyingLight]UserData (?) | metatable
                        lua_pushstring(luaState, gkInheritanceKey);
                        // stack: [underlyingLight]UserData (?) | metatable | gkInheritanceKey
                        lua_rawget(luaState, -2);
                        // stack: [underlyingLight]UserData (?) | metatable | inheritanceTable (?)
                        if (lua_istable(luaState, -1) != 0) {
                            // stack: [underlyingLight]UserData (?) | metatable | inheritanceTable
                            if (isRecursion == false) {
                                // stack: userData (?) | metatable | inheritanceTable
                                lua_pushvalue(luaState, -2);
                                // stack: userdata (?) | metatable | inheritanceTable | metatable
                                if (lua_isuserdata(luaState, -4) != 0) {
                                    // stack: userdata | metatable | inheritanceTable | metatable
                                    if (pushUnderlyingTypeLightUserData(luaState, -4) == true) {
                                        // stack: userdata | metatable | inheritanceTable | underlyingLightUserData
                                        lua_remove(luaState, -4);
                                        // stack: metatable | inheritanceTable | underlyingLightUserData
                                        lua_insert(luaState, -3);
                                        // stack: underlyingLightUserData | metatable | inheritanceTable
                                        // [*]
                                    } else {
                                        // stack: userdata | metatable | inheritanceTable
                                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted UnderlyingTypeFunction");
                                    }
                                } else {
                                    // stack: ? | metatable | inheritanceTable | metatable
                                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "failed to push convertible or inherited type: value at index -4 is not userdata");
                                }
                            }
                            // [-]
                            // stack: underlyingLightUserData | metatable | inheritanceTable
                            for (std::size_t i = lua_compatibility::rawlen(luaState, -1); i >= 1; --i) {
                                // stack: underlyingLightUserData | metatable | inheritanceTable
                                lua_compatibility::pushunsigned(luaState, i);
                                // stack: underlyingLightUserData | metatable | inheritanceTable | i
                                lua_rawget(luaState, -2);
                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable (?)
                                if (lua_istable(luaState, -1) != 0) {
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable
                                    lua_rawgeti(luaState, -1, static_cast<lua_Integer>(InheritanceTable::kMetatableIndex));
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable (?)
                                    if (lua_istable(luaState, -1) != 0) {
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable
                                        lua_rawgeti(luaState, -2, static_cast<lua_Integer>(InheritanceTable::kTypeIndexIndex));
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | baseTypeIndex (?)
                                        const std::type_index *baseTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -1, gkTypeIndexMetatableName));
                                        if (baseTypeIndex != nullptr) {
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | baseTypeIndex
                                            lua_pop(luaState, 1);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable
                                            lua_remove(luaState, -2);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable
                                            lua_pushvalue(luaState, -3);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable | metatable
                                            if (pushDirectConvertibleType(luaState, -5, *baseTypeIndex) == true) {
                                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable | baseClassLightUserData
                                                lua_insert(luaState, -2);
                                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseClassLightUserData | baseMetatable
                                                tagInheritanceSearch(luaState, -4);
                                                if (pushConvertibleOrInheritedType(luaState, -2, convertibleTypeIndex, true) == true) {
                                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseClassLightUserData | lightUserData
                                                    untagInheritanceSearch(luaState, -4);
                                                    lua_insert(luaState, -5);
                                                    // stack: lightUserData | underlyingLightUserData | metatable | inheritanceTable | baseClassLightUserData
                                                    lua_pop(luaState, 4);
                                                    return true;
                                                } else {
                                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseClassLightUserData
                                                    untagInheritanceSearch(luaState, -3);
                                                    lua_pop(luaState, 1);
                                                    // stack: underlyingLightUserData | metatable | inheritanceTable
                                                    // [*]
                                                }
                                            } else {
                                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable
                                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: there should be a conversion function for a inherited type");
                                            }
                                        } else {
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | ?
                                            throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: expected baseTypeIndex at index -1");
                                        }
                                    } else {
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | ?
                                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: expected baseMetatable at index -1");

                                    }
                                } else {
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | ?
                                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted inheritanceTable: expected baseTable at index -1");
                                }
                                // [-]
                                // stack: underlyingLightUserData | metatable | inheritanceTable
                            }
                            // stack: underlyingLightUserData | metatable | inheritanceTable
                            lua_pop(luaState, 3);
                            return false;
                        } else {
                            // stack: underlyingLightUserData | metatable | nil (?)
                            lua_pop(luaState, 3);
                            return false;
                        }
                    }
                } else { // avoids infinite recursion
                    // stack: metatable
                    lua_pop(luaState, 1);
                    return false;
                }
            }

            UserDataWrapperBase * getUserDataWrapperBase(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: userdata (?)
                if (lua_getmetatable(luaState, -1) != 0) {
                    // stack: userdata (?) | metatable
                    lua_pushstring(luaState, gkUserDataWrapperBaseTableKey);
                    // stack: userdata (?) | metatable | gkUserDataWrapperBaseTableKey
                    lua_rawget(luaState, -2);
                    // stack: userdata (?) | metatable | userDataWrapperBaseTable (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: userdata (?) | metatable | userDataWrapperBaseTable
                        lua_rawgeti(luaState, -1, static_cast<lua_Integer>(UserDataWrapperBaseTable::kTypeIndexIndex));
                        // stack: userdata (?) | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex (?)
                        const std::type_index *typeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -1, gkTypeIndexMetatableName));
                        if (typeIndex != nullptr) {
                            // stack: userdata (?) | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex
                            if (*typeIndex == std::type_index(typeid(UserDataWrapperBase))) {
                                // stack: userdata (?) | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex
                                lua_pop(luaState, 1);
                                // stack: userdata (?) | metatable | userDataWrapperBaseTable
                                lua_rawgeti(luaState, -1, static_cast<lua_Integer>(UserDataWrapperBaseTable::kFunctionIndex));
                                // stack: userdata (?) | metatable | userDataWrapperBaseTable | function (?)
                                if (lua_iscfunction(luaState, -1) != 0) {
                                    // stack: userdata (?) | metatable | userDataWrapperBaseTable | function
                                    lua_pushvalue(luaState, -4);
                                    // stack: userdata (?) | metatable | userDataWrapperBaseTable | function | userdata (?)
                                    if (lua_isuserdata(luaState, -1) != 0) {
                                        // stack: userdata | metatable | userDataWrapperBaseTable | function | userdata
                                        lua_call(luaState, 1, 1);
                                        // stack: userdata | metatable | userDataWrapperBaseTable | UserDataWrapperBaseLightUserData (?)
                                        if (lua_islightuserdata(luaState, -1) != 0) {
                                            // stack: userdata | metatable | userDataWrapperBaseTable | UserDataWrapperBaseLightUserData
                                            UserDataWrapperBase *userDataWrapperBase = static_cast<UserDataWrapperBase *>(lua_touserdata(luaState, -1));
                                            lua_pop(luaState, 4);
                                            return userDataWrapperBase;
                                        } else {
                                            // stack: userdata | metatable | userDataWrapperBaseTable | ?
                                            throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted userDataWrapperBaseTable: expected UserDataWrapperBaseLightUserData at index -1");
                                        }
                                    } else {
                                        // stack: ? | metatable | userDataWrapperBaseTable | function | ?
                                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted userDataWrapperBaseTable: expected userdata at index -1");
                                    }
                                } else {
                                    // stack: userdata (?) | metatable | userDataWrapperBaseTable | ?
                                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted userDataWrapperBaseTable: expected cfunction at index -1");
                                }
                            } else {
                                // stack: userdata (?) | metatable | userDataWrapperBaseTable | ? (wrongTypeIndex - incompatible integral library?)
                                lua_pop(luaState, 4);
                            }
                        } else {
                            // stack: userdata (?) | metatable | userDataWrapperBaseTable | ?
                            lua_pop(luaState, 4);
                        }
                    } else {
                        // stack: userdata (?) | metatable | nil (?)
                        lua_pop(luaState, 3);
                    }
                } else {
                    // stack: userdata (?)
                    lua_pop(luaState, 1);
                }
                return nullptr;
            }

            bool pushUnderlyingTypeLightUserData(lua_State *luaState, int index) {
                // stack: metatable
                lua_pushvalue(luaState, index);
                // stack: metatable | userdata
                lua_pushstring(luaState, gkUnderlyingTypeFunctionKey);
                // stack: metatable | userdata | gkUnderlyingTypeFunctionKey
                lua_rawget(luaState, -3);
                // stack: metatable | userdata | function (?)
                if (lua_iscfunction(luaState, -1) != 0) {
                    // stack: metatable | userdata | function
                    lua_insert(luaState, -2);
                    // stack: metatable | function | userdata
                    lua_call(luaState, 1, 1);
                    // stack: metatable | underlyingLightUserData
                    lua_remove(luaState, -2);
                    // stack: underlyingLightUserData
                    return true;
                } else {
                    // stack: metatable | userdata | nil (?)
                    lua_pop(luaState, 3);
                    return false;
                }
            }


            // lua_CFunction style function: no exceptions and no objects (with destructors)
            // noexcept specifier is not used because if lua is compiled as c++, some of its functions might throw an exception
            int callInheritanceIndexMetamethod(lua_State *luaState) {
                // arguments: table (1), key (2)
                if (checkInheritanceSearchTag(luaState, 1) == false) {
                    lua_pushvalue(luaState, 2);
                    // stack: key
                    lua_rawget(luaState, 1);
                    // stack: returnValue or nil
                    if (lua_isnil(luaState, -1) != 0) {
                        // stack: nil
                        lua_pop(luaState, 1);
                        // stack:
                        lua_pushstring(luaState, gkInheritanceKey);
                        // stack: gkInheritanceKey
                        lua_rawget(luaState, 1);
                        // stack: inheritanceTable (?)
                        if (lua_istable(luaState, -1) != 0) {
                            // stack: inheritanceTable
                            for (std::size_t i = lua_compatibility::rawlen(luaState, -1); i >= 1; --i) {
                                // stack: inheritanceTable
                                lua_compatibility::pushunsigned(luaState, i);
                                // stack: inheritanceTable | i
                                lua_rawget(luaState, -2);
                                // stack: inheritanceTable | baseTable (?)
                                if (lua_istable(luaState, -1) != 0) {
                                    // stack: inheritanceTable | baseTable
                                    lua_rawgeti(luaState, -1, static_cast<lua_Integer>(InheritanceTable::kMetatableIndex));
                                    // stack: inheritanceTable | baseTable | baseMetatable (?)
                                    if (lua_istable(luaState, -1) != 0) {
                                        // stack: inheritanceTable | baseTable | baseMetatable
                                        lua_pushvalue(luaState, 2);
                                        // stack: inheritanceTable | baseTable | baseMetatable | key
                                        tagInheritanceSearch(luaState, 1);
                                        // lua_gettable invokes baseMetatable __index metamethod, causing callInheritanceIndexMetamethod to be called again. The argument metatable is tagged to avoid infinite recursion
                                        lua_gettable(luaState, -2);
                                        untagInheritanceSearch(luaState, 1);
                                        // stack: inheritanceTable | baseTable | baseMetatable | returnValue (?)
                                        if (lua_isnil(luaState, -1) == 0) {
                                            // stack: inheritanceTable | baseTable | baseMetatable | returnValue
                                            lua_insert(luaState, -4);
                                            // stack: returnValue | inheritanceTable | baseTable | baseMetatable
                                            lua_pop(luaState, 3);
                                            // stack: returnValue
                                            return 1;
                                        } else {
                                            // stack: inheritanceTable | baseTable | baseMetatable | nil
                                            lua_pop(luaState, 3);
                                            // stack: inheritanceTable
                                        }
                                    } else {
                                        // stack: inheritanceTable | baseTable | ?
                                        lua_pop(luaState, 3);
                                        // stack:
                                        lua_pushstring(luaState, "[integral] corrupted inheritance table - 1");
                                        // stack: errorString
                                        return lua_error(luaState);
                                    }
                                } else {
                                    // stack: inheritanceTable | ?
                                    lua_pop(luaState, 2);
                                    // stack:
                                    lua_pushstring(luaState, "[integral] corrupted inheritance table - 2");
                                    // stack: errorString
                                    return lua_error(luaState);
                                }
                            }
                            // stack: inheritanceTable
                            lua_pop(luaState, 1);
                            // stack:
                            lua_pushnil(luaState);
                            // stack: nil
                            return 1;
                        } else {
                            // stack: ?
                            lua_pop(luaState, 1);
                            // stack:
                            lua_pushstring(luaState, "[integral] corrupted inheritance table - 3");
                            // stack: errorString
                            return lua_error(luaState);
                        }
                    } else {
                        // stack: returnValue
                        return 1;
                    }
                } else {
                    lua_pushnil(luaState);
                    // stack: nil
                    return 1;
                }
            }

            void pushInheritanceIndexMetamethod(lua_State *luaState) {
                lua_pushstring(luaState, gkInheritanceIndexMetamethodKey);
                // stack: gkInheritanceIndexMetamethodKey
                lua_rawget(luaState, LUA_REGISTRYINDEX);
                // stack: callInheritanceIndexMetamethod (?)
                if (lua_iscfunction(luaState, -1) == 0) {
                    // stack: ?
                    lua_pop(luaState, 1);
                    // stack:
                    lua_pushcfunction(luaState, callInheritanceIndexMetamethod);
                    // stack: callInheritanceIndexMetamethod
                    lua_pushstring(luaState, gkInheritanceIndexMetamethodKey);
                    // stack: callInheritanceIndexMetamethod | gkInheritanceIndexMetamethodKey
                    lua_pushvalue(luaState, -2);
                    // stack: callInheritanceIndexMetamethod | gkInheritanceIndexMetamethodKey | callInheritanceIndexMetamethod
                    lua_rawset(luaState, LUA_REGISTRYINDEX);
                    // stack: callInheritanceIndexMetamethod
                }
            }

            void setInheritanceIndexMetatable(lua_State *luaState) {
                // stack: metatable
                if (lua_getmetatable(luaState, -1) == 0) {
                    // stack: metatable
                    lua_createtable(luaState, 0, 1);
                    // stack: metatable | metatableMetatable*
                    lua_pushstring(luaState, "__index");
                    // stack: metatable | metatableMetatable* | "__index"
                    pushInheritanceIndexMetamethod(luaState);
                    // stack: metatable | metatableMetatable* | "__index" | callInheritanceIndexMetamethod
                    lua_rawset(luaState, -3);
                    // stack: metatable | metatableMetatable*
                    lua_setmetatable(luaState, -2);
                    // stack: metatable
                } else { // preserves a previously defined metatable (possibly with other metamethods)
                    // stack: metatable | metatableMetatable
                    lua_pushstring(luaState, "__index");
                    // stack: metatable | metatableMetatable | "__index"
                    pushInheritanceIndexMetamethod(luaState);
                    // stack: metatable | metatableMetatable | "__index" | callInheritanceIndexMetamethod
                    lua_rawset(luaState, -3);
                    // stack: metatable | metatableMetatable
                    lua_pop(luaState, 1);
                    // stack: metatable
                }
            }
        }
    }
}

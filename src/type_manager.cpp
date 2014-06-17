//
//  type_manager.cpp
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

#include "type_manager.h"
#include <typeindex>
#include <lua.hpp>
#include "basic.h"
#include "RuntimeException.h"

namespace integral {
    namespace detail {
        namespace type_manager {
            const char * const gkTypeMangerRegistryKey = "integral_TypeManagerRegistryKey";
            const char * const gkTypeIndexKey = "integral_TypeIndexKey";
            const char * const gkTypeManagerVersionKey = "integral_TypeManagerVersionKey";
            const char * const gkTypeManagerVersion = "0.1";
            const char * const gkTypeIndexMetatableName = "integral_TypeIndexMetatableName";
            const char * const gkTypeFunctionsKey = "integral_TypeFunctionsKey";
            const char * const gkUserDataWrapperBaseTableKey = "integral_UserDataWrapperBaseTableKey";
            const char * const gkUnderlyingTypeFunctionKey = "integral_UnderlyingTypeFunctionKey";
            const char * const gkInheritanceSearchTagKey = "integral_InheritanceSearchTagKey";
            const char * const gkInheritanceKey = "integral_InheritanceKey";
            const char * const gkInheritanceIndexMetatable = "integral_InheritanceIndexMetatable";

            
            const std::type_index * getClassMetatableType(lua_State *luaState) {
                //stack: metatable
                lua_pushstring(luaState, gkTypeIndexKey);
                //stack: metatable | gkTypeIndexKey
                lua_rawget(luaState, -2);
                if (lua_isuserdata(luaState, -1) != 0) {
                    //stack: metatable | type_index (?)
                    const std::type_index *userDataTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -1, gkTypeIndexMetatableName));
                    if (userDataTypeIndex != nullptr) {
                        //stack: metatable | type_index
                        lua_pop(luaState, 1);
                        //stack: metatable
                        return userDataTypeIndex;
                    } else {
                        //stack: metatable | ???
                        lua_pop(luaState, 1);
                        throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted userdata type_index");
                    }
                } else {
                    //stack: metatable | ???
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
                lua_pushstring(luaState, gkTypeMangerRegistryKey);
                lua_rawget(luaState, LUA_REGISTRYINDEX);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: typeManager
                    lua_pushinteger(luaState, typeIndex.hash_code());
                    lua_rawget(luaState, -2);
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: typeManager | typeFunctionHashTable
                        lua_remove(luaState, -2);
                        // stack: typeFunctionHashTable
                        lua_pushnil(luaState);
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -2, gkTypeIndexMetatableName));
                            // stack: typeFunctionHashTable | type_index_udata | rootMetatable
                            if (storedTypeIndex != nullptr) {
                                if (*storedTypeIndex == typeIndex) {
                                    lua_pop(luaState, 3);
                                    return true;
                                }
                            } else {
                                lua_pop(luaState, 3);
                                throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeManager - checkClassMetatableExistence");
                            }
                            // stack: typeFunctionHashTable | type_index_udata | rootMetatable
                        }
                        // stack: typeFunctionHashTable
                        lua_pop(luaState, 1);
                        return false;
                    } else {
                        // stack: typeManager | nil
                        lua_pop(luaState, 2);
                        return false;
                    }
                } else {
                    // stack: nil
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
                lua_pushinteger(luaState, baseTypeHash);
                lua_newtable(luaState);
                lua_pushvalue(luaState, -1);
                // stack: metatable | typeFunctionTable | typeHash | typeFunctionHashTable* | typeFunctionHashTable*
                lua_insert(luaState, -4);
                // stack: metatable | typeFunctionHashTable* | typeFunctionTable | typeHash | typeFunctionHashTable*
                lua_rawset(luaState, -3);
                // stack: metatable | typeFunctionHashTable | typeFunctionTable
                lua_pop(luaState, 1);
                // stack: metatable | typeFunctionHashTable
            }
            
            void pushTypeFunctionHashTable(lua_State *luaState, const std::type_index &baseTypeIndex) {
                // stack: metatable
                const std::size_t baseTypeHash = baseTypeIndex.hash_code();
                lua_pushstring(luaState, gkTypeFunctionsKey);
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: metatable | typeFunctionTable
                    lua_pushinteger(luaState, baseTypeHash);
                    lua_rawget(luaState, -2);
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: metatable | typeFunctionTable | typeFunctionHashTable
                        lua_remove(luaState, -2);
                        // stack: metatable | typeFunctionHashTable
                    } else {
                        // stack:  metatable | typeFunctionTable | nil
                        lua_pop(luaState, 1);
                        setTypeFunctionHashTable(luaState, baseTypeHash);
                        // stack: metatable | typeFunctionHashTable
                    }
                } else {
                    // stack: metatable | nil
                    lua_pop(luaState, 1);
                    lua_newtable(luaState);
                    lua_pushstring(luaState, gkTypeFunctionsKey);
                    // stack: metatable | typeFunctionTable* | gkTypeFunctionsKey
                    lua_pushvalue(luaState, -2);
                    // stack: metatable | typeFunctionTable* | gkTypeFunctionsKey | typeFunctionTable*
                    lua_rawset(luaState, -4);
                    // stack: metatable | typeFunctionTable
                    setTypeFunctionHashTable(luaState, baseTypeHash);
                    // stack: metatable | typeFunctionHashTable
                }
            }
            
            // not allowed to throw exception (this function is used in callIndexMetamethod)
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
                    return false;
                } else {
                    // stack: metatable | true
                    return true;
                }
            }
            
            // not allowed to throw exception (this function is used in callIndexMetamethod)
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
            }
            
            // not allowed to throw exception (this function is used in callIndexMetamethod)
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
            }
            
            bool pushDirectConvertibleType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isUnderlyingLightUserData) {
                // stack: metatable
                lua_pushvalue(luaState, index);
                // stack: metatable | [underlyingLight]UserData
                lua_insert(luaState, -2);
                // stack: [underlyingLight]UserData | metatable
                lua_pushstring(luaState, gkTypeFunctionsKey);
                // stack: [underlyingLight]UserData | metatable | gkTypeFunctionsKey
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) != 0) {
                    // stack: [underlyingLight]UserData | metatable | typeFunctionTable
                    lua_pushinteger(luaState, convertibleTypeIndex.hash_code());
                    // stack: [underlyingLight]UserData | metatable | typeFunctionTable | typeHash
                    lua_rawget(luaState, -2);
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: [underlyingLight]UserData | metatable | typeFunctionTable | typeFunctionHashTable
                        lua_remove(luaState, -2);
                        // stack: [underlyingLight]UserData | metatable | typeFunctionHashTable
                        lua_pushnil(luaState);
                        // stack: [underlyingLight]UserData | metatable | typeFunctionHashTable | nil
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            // stack: [underlyingLight]UserData | metatable | typeFunctionHashTable | type_index_udata (?) | function (?)
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -2, gkTypeIndexMetatableName));
                            if (storedTypeIndex != nullptr) {
                                if (*storedTypeIndex == convertibleTypeIndex) {
                                    // stack: [UnderlyingLight]UserData | metatable | typeFunctionHashTable | type_index_udata | function (?)
                                    lua_remove(luaState, -2);
                                    // stack: [UnderlyingLight]UserData | metatable | typeFunctionHashTable | function (?)
                                    lua_remove(luaState, -2);
                                    // stack: [UnderlyingLight]UserData | metatable | function (?)
                                    if (lua_iscfunction(luaState, -1) != 0) {
                                        // stack: [UnderlyingLight]UserData | metatable | function
                                        if (isUnderlyingLightUserData == false) {
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
                                                lua_pop(luaState, 2);
                                                throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted UnderlyingTypeFunction - pushDirectConvertibleType");
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
                                            return true;
                                        } else {
                                            // stack: ???
                                            lua_pop(luaState, 1);
                                            throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeFunction typeFunctionHashTable - 1");
                                        }
                                    } else {
                                        // stack: [UnderlyingLight]UserData | metatable | ???
                                        lua_pop(luaState, 3);
                                        throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeFunction typeFunctionHashTable - 2");
                                    }
                                }
                            } else {
                                // stack: underlyingLightUserData | metatable | typeFunctionHashTable | ? | function (?)
                                lua_pop(luaState, 5);
                                throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeFunction typeFunctionHashTable - 3");
                            }
                            // stack: underlyingLightUserData | metatable | typeFunctionHashTable | type_index_udata | function
                        }
                        // stack: [underlyingLight]UserData | metatable | typeFunctionHashTable
                        lua_pop(luaState, 3);
                    } else {
                        // stack: [underlyingLight]UserData | metatable | typeFunctionTable | nil
                        lua_pop(luaState, 4);
                    }
                } else {
                    // stack: [underlyingLight]UserData | metatable | nil
                    lua_pop(luaState, 3);
                }
                return false;
            }
            
            bool pushConvertibleOrInheritedType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isRecursion) {
                // stack: metatable
                if (isRecursion == true && checkInheritanceSearchTag(luaState, -1) == true) {
                    // stack: metatable
                    lua_pop(luaState, 1);
                    return false;
                } else {
                    // stack: metatable
                    lua_pushvalue(luaState, index);
                    // stack: metatable | [underlyingLight]UserData
                    lua_insert(luaState, -2);
                    // stack: [underlyingLight]UserData | metatable
                    lua_pushvalue(luaState, -1);
                    // stack: [underlyingLight]UserData | metatable | metatable
                    if (pushDirectConvertibleType(luaState, -3, convertibleTypeIndex, isRecursion) == true) {
                        // stack: underlyingLightUserData | metatable | convertedLightUserData
                        lua_remove(luaState, -2);
                        lua_remove(luaState, -2);
                        return true;
                    } else {
                        // stack: [underlyingLight]UserData | metatable
                        lua_pushstring(luaState, gkInheritanceKey);
                        // stack: [underlyingLight]UserData | metatable | gkInheritanceKey
                        lua_rawget(luaState, -2);
                        if (lua_istable(luaState, -1) != 0) {
                            // stack: [underlyingLight]UserData | metatable | inheritanceTable
                            if (isRecursion == false) {
                                // stack: userData | metatable | inheritanceTable
                                lua_pushvalue(luaState, -2);
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
                                    lua_pop(luaState, 3);
                                    throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted UnderlyingTypeFunction - pushConvertibleOrInheritedType");
                                }
                            }
                            // [-]
                            // stack: underlyingLightUserData | metatable | inheritanceTable
                            for (std::size_t i = lua_rawlen(luaState, -1); i >= 1; --i) {
                                // stack: underlyingLightUserData | metatable | inheritanceTable
                                lua_pushunsigned(luaState, i);
                                // stack: underlyingLightUserData | metatable | inheritanceTable | i
                                lua_rawget(luaState, -2);
                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable (?)
                                if (lua_istable(luaState, -1) != 0) {
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable
                                    lua_rawgeti(luaState, -1, keInheritanceMetatableIndex);
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable (?)
                                    if (lua_istable(luaState, -1) != 0) {
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable
                                        lua_rawgeti(luaState, -2, keInheritanceTypeIndexIndex);
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | baseTypeIndex (?)
                                        const std::type_index *baseTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -1, gkTypeIndexMetatableName));
                                        if (baseTypeIndex != nullptr) {
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | baseTypeIndex
                                            lua_pop(luaState, 1);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable
                                            lua_remove(luaState, -2);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable
                                            lua_pushvalue(luaState, -3);
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable | metatable
                                            if (pushDirectConvertibleType(luaState, -5, *baseTypeIndex, isRecursion) == true) {
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
                                                }
                                            } else {
                                                // stack: underlyingLightUserData | metatable | inheritanceTable | baseMetatable
                                                lua_pop(luaState, 4);
                                                // there should be a conversion function for a inherited type
                                                throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeFunction inheritanceTable - pushInheritedConvertibleType - 1");
                                            }
                                        } else {
                                            // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | baseMetatable | ???
                                            lua_pop(luaState, 6);
                                            throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted inheritanceTable - pushInheritedConvertibleType - 2");
                                        }
                                    } else {
                                        // stack: underlyingLightUserData | metatable | inheritanceTable | baseTable | ???
                                        lua_pop(luaState, 5);
                                        throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted inheritanceTable - pushInheritedConvertibleType - 3");

                                    }
                                } else {
                                    // stack: underlyingLightUserData | metatable | inheritanceTable | ???
                                    lua_pop(luaState, 4);
                                    throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted TypeFunction inheritanceTable - pushInheritedConvertibleType - 4");
                                }
                            }
                            // stack: underlyingLightUserData | metatable | inheritanceTable
                            lua_pop(luaState, 3);
                            return false;
                        } else {
                            // stack: underlyingLightUserData | metatable | nil
                            lua_pop(luaState, 3);
                            return false;
                        }
                    }
                }
            }
            
            UserDataWrapperBase * getUserDataWrapperBase(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: userdata
                if (lua_getmetatable(luaState, -1) != 0) {
                    // stack: userdata | metatable
                    lua_pushstring(luaState, gkUserDataWrapperBaseTableKey);
                    // stack: userdata | metatable | gkUserDataWrapperBaseTableKey
                    lua_rawget(luaState, -2);
                    // stack: userdata | metatable | userDataWrapperBaseTable (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: userdata | metatable | userDataWrapperBaseTable
                        lua_rawgeti(luaState, -1, keUserDataWrapperBaseTypeIndexIndex);
                        // stack: userdata | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex (?)
                        const std::type_index *typeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -1, gkTypeIndexMetatableName));
                        if (typeIndex != nullptr) {
                            // stack: userdata | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex
                            if (*typeIndex == std::type_index(typeid(UserDataWrapperBase)) == true) {
                                // stack: userdata | metatable | userDataWrapperBaseTable | userDataWrapperBaseTypeIndex
                                lua_pop(luaState, 1);
                                // stack: userdata | metatable | userDataWrapperBaseTable
                                lua_rawgeti(luaState, -1, keUserDataWrapperBaseFunctionIndex);
                                // stack: userdata | metatable | userDataWrapperBaseTable | function (?)
                                if (lua_iscfunction(luaState, -1) != 0) {
                                    // stack: userdata | metatable | userDataWrapperBaseTable | function
                                    lua_pushvalue(luaState, -4);
                                    // stack: userdata | metatable | userDataWrapperBaseTable | function | userdata
                                    lua_call(luaState, 1, 1);
                                    // stack: userdata | metatable | userDataWrapperBaseTable | UserDataWrapperBaseLightUserData (?)
                                    if (lua_islightuserdata(luaState, -1) != 0) {
                                        // stack: userdata | metatable | userDataWrapperBaseTable | UserDataWrapperBaseLightUserData
                                        UserDataWrapperBase *userDataWrapperBase = static_cast<UserDataWrapperBase *>(lua_touserdata(luaState, -1));
                                        lua_pop(luaState, 4);
                                        return userDataWrapperBase;
                                    } else {
                                        // stack: userdata | metatable | userDataWrapperBaseTable | ???
                                        lua_pop(luaState, 4);
                                        throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted userDataWrapperBaseTable - 1");
                                    }
                                } else {
                                    // stack: userdata | metatable | userDataWrapperBaseTable | ???
                                    lua_pop(luaState, 4);
                                    throw RuntimeException(__FILE__, __LINE__, __func__, "corrupted userDataWrapperBaseTable - 2");
                                }
                            } else {
                                // stack: userdata | metatable | userDataWrapperBaseTable | ??? (wrongTypeIndex - incompatible integral library?)
                                lua_pop(luaState, 4);
                            }
                        } else {
                            // stack: userdata | metatable | userDataWrapperBaseTable | ???
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
            
            bool pushUnderlyingTypeLightUserData(lua_State *luaState, int index) {
                // stack: metatable
                lua_pushvalue(luaState, index);
                // stack: metatable | userdata
                lua_pushstring(luaState, gkUnderlyingTypeFunctionKey);
                // stack: metatable | userdata | gkUnderlyingTypeFunctionKey
                lua_rawget(luaState, -3);
                // stack: metatable | userdata | function (?)
                if (lua_iscfunction(luaState, -1) == true) {
                    // stack: metatable | userdata | function
                    lua_insert(luaState, -2);
                    // stack: metatable | function | userdata
                    lua_call(luaState, 1, 1);
                    // stack: metatable | underlyingLightUserData
                    lua_remove(luaState, -2);
                    return true;
                } else {
                    // stack: metatable | userdata | ???
                    lua_pop(luaState, 3);
                    return false;
                }
            }


            // lua_CFunction style function: no exceptions and no objects (with destructors)
            int callIndexMetamethod(lua_State *luaState) {
                // arguments: table (1), key (2)
                if (checkInheritanceSearchTag(luaState, -2) == false) {
                    lua_pushvalue(luaState, 2);
                    // stack: key
                    lua_rawget(luaState, 1);
                    // stack: returnValue or nil
                    if (lua_isnil(luaState, -1) != 0) {
                        // stack: nil
                        lua_pop(luaState, 1);
                        lua_pushstring(luaState, gkInheritanceKey);
                        // stack: gkInheritanceKey
                        lua_rawget(luaState, 1);
                        if (lua_istable(luaState, -1) != 0) {
                            // stack: inheritanceTable
                            for (std::size_t i = lua_rawlen(luaState, -1); i >= 1; --i) {
                                // stack: inheritanceTable
                                lua_pushunsigned(luaState, i);
                                // stack: inheritanceTable | i
                                lua_rawget(luaState, -2);
                                if (lua_istable(luaState, -1) != 0) {
                                    // stack: inheritanceTable | baseTable
                                    lua_rawgeti(luaState, -1, keInheritanceMetatableIndex);
                                    // stack: inheritanceTable | baseTable | baseMetatable (?)
                                    if (lua_istable(luaState, -1) != 0) {
                                        // stack: inheritanceTable | baseTable | baseMetatable
                                        lua_pushvalue(luaState, 2);
                                        // stack: inheritanceTable | baseTable | baseMetatable | key
                                        tagInheritanceSearch(luaState, 1);
                                        lua_gettable(luaState, -2);
                                        untagInheritanceSearch(luaState, 1);
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
                                        // stack: inheritanceTable | baseTable | ???
                                        lua_pop(luaState, 3);
                                        lua_pushstring(luaState, "[integral] corrupted inheritance table - 1");
                                        return lua_error(luaState);
                                    }
                                } else {
                                    // stack: inheritanceTable | ???
                                    lua_pop(luaState, 2);
                                    lua_pushstring(luaState, "[integral] corrupted inheritance table - 2");
                                    return lua_error(luaState);
                                }
                            }
                            // stack: inheritanceTable
                            lua_pop(luaState, 1);
                            lua_pushnil(luaState);
                            // stack: nil
                            return 1;
                        } else {
                            // stack: ???
                            lua_pop(luaState, 1);
                            lua_pushstring(luaState, "[integral] corrupted inheritance table - 3");
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
            
            void pushInheritanceIndexMetaTable(lua_State *luaState) {
                // stack: metatable
                lua_pushstring(luaState, gkInheritanceIndexMetatable);
                // stack: gkInheritanceIndexMetatable
                lua_rawget(luaState, LUA_REGISTRYINDEX);
                if (lua_istable(luaState, -1) == 0) {
                    // stack: nil
                    lua_pop(luaState, 1);
                    lua_newtable(luaState);
                    lua_pushstring(luaState, gkInheritanceIndexMetatable);
                    // stack: InheritanceMetatable* | gkInheritanceIndexMetatable
                    lua_pushvalue(luaState, -2);
                    // stack: InheritanceMetatable* | gkInheritanceIndexMetatable | InheritanceMetatable*
                    lua_rawset(luaState, LUA_REGISTRYINDEX);
                    // stack: InheritanceMetatable
                    basic::setLuaFunction(luaState, "__index", callIndexMetamethod, 0);
                }
                // stack: InheritanceMetatable
            }
        }
    }
}

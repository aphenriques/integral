//
//  type_manager.h
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

#ifndef integral_type_manager_h
#define integral_type_manager_h

#include <stdexcept>
#include <functional>
#include <typeindex>
#include <lua.hpp>
#include "basic.h"
#include "UserDataWrapper.h"

// typeid(T).name() cannot be used as an automatic indentifier for metatables. It is not safe: different classes might have the same typeid(T).name() (c++ standard)
// std::type_index is used as an automatic indentifier for metatables. Supposedly, it is safe.

// TypeManager organization (hashmap):
// REGISTRY[gkTypeMangerRegistryKey] = {[typeHash] = {[type_index_udata] = rootMetatable}}
// REGISTRY[gkTypeMangerRegistryKey] = {[typeHash] = typeFunctionHashTable}
// REGISTRY[gkTypeMangerRegistryKey] = typeManager

// TypeManager (integral) version control:
// REGISTRY[gkTypeMangerRegistryKey][gkTypeManagerVersionKey] = TYPE_MANAGER_VERSION

// there might be clashes with typeHash. It is NOT a problem. TypeManager is implemented as a hashmap.

// Attention! The stored type_index (type_index_udata) is UserDataWrapper<T>
// This is useful when multiple integral versions are used.
// Maybe UserDataWrapper<T> is incompatible from different integral versions used together; this way, it will fail gracefully.

namespace integral {
    namespace detail {
        namespace type_manager {
            extern const char * const gkTypeMangerRegistryKey;
            
            extern const char * const gkTypeIndexKey;
            
            extern const char * const gkTypeManagerVersionKey;
            
            extern const char * const gkTypeIndexMetatableName;
            
            extern const char * const gkTypeFunctionsKey;
            
            extern const char * const gkUserDataWrapperBaseTableKey;
            
            extern const char * const gkUnderlyingTypeFunctionKey;
            
            extern const char * const gkInheritanceSearchTagKey;
            
            extern const char * const gkInheritanceKey;
            
            extern const char * const gkInheritanceIndexMetatable;
            
            constexpr unsigned TYPE_MANAGER_VERSION = 1;
            
            // FIXME change this constants to enums
            
            constexpr int INHERITANCE_TYPE_INDEX_INDEX = 1;
            
            constexpr int INHERITANCE_METATABLE_INDEX = 2;
            
            constexpr int USERDATAWRAPPERBASE_TYPE_INDEX_INDEX = 1;
            
            constexpr int USERDATAWRAPPERBASE_FUNCTION_INDEX = 2;
            
            const std::type_index * getClassMetatableType(lua_State *luaState);
            
            bool checkClassMetatableType(lua_State *luaState, const std::type_index &typeIndex);
            
            // type index is the netatable type (type = typeid(UserDataWrapper<T>))
            bool checkClassMetatableExistence(lua_State *luaState, const std::type_index &typeIndex);
            
            // T is the underlying type
            template<typename T>
            inline bool checkClassMetatableExistence(lua_State *luaState);
            
            template<typename T>
            UserDataWrapper<T> * getUserDataWrapper(lua_State *luaState, int index);
            
            template<typename T>
            void pushRootMetatableFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex);
            
            template<typename T>
            void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash);
            
            template<typename T>
            bool pushClassMetatable(lua_State *luaState);
            
            void pushTypeIndexUserData(lua_State *luaState, const std::type_index &typeIndex);
            
            template<typename T>
            inline void pushTypeIndexUserData(lua_State *luaState);
            
            void setTypeFunctionHashTable(lua_State *luaState, size_t baseTypeHash);
            
            void pushTypeFunctionHashTable(lua_State *luaState, const std::type_index &baseTypeIndex);
            
            template<typename D, typename B>
            void setTypeFunction(lua_State *luaState);
            
            template<typename T, typename U>
            void setTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
            
            template<typename D, typename B>
            void defineTypeFunction(lua_State *luaState);
            
            template<typename T, typename U>
            void defineTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
            
            // protects from recursion (although in a very unlikely scenario)
            // index: metatable to be tagged
            bool checkInheritanceSearchTag(lua_State *luaState, int index);

            // protects from recursion (although in a very unlikely scenario)
            // index: metatable to be tagged
            void tagInheritanceSearch(lua_State *luaState, int index);
            
            // protects from recursion (although in a very unlikely scenario)
            // index: metatable to be tagged
            void untagInheritanceSearch(lua_State *luaState, int index);
            
            // index: userdata (isUnderlyingLightUserData = false) or lightuserdata (isUnderlyingLightUserData = true) of the underlying type of a userdata stack index
            // stack argument: metatable
            // returns true if the convertible type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushDirectConvertibleType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isUnderlyingLightUserData);
            
            // index: userdata (recursion = false) or lightuserdata (recursion = true) of the underlying type of a userdata stack index
            // stack argument: metatable
            // returns true if the convertible type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushConvertibleOrInheritedType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isRecursion);
            
            template<typename T>
            void setUserDataWrapperBaseTable(lua_State *luaState);
            
            UserDataWrapperBase * getUserDataWrapperBase(lua_State *luaState, int index);
            
            // stack argument: metatable
            // returns true if the underlying type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushUnderlyingTypeLightUserData(lua_State *luaState, int index);

            template<typename T>
            T * getConvertibleType(lua_State *luaState, int index);

            template<typename D, typename B>
            void setInheritance(lua_State *luaState);
            
            template<typename T, typename U>
            void setInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
            
            void defineInheritance(lua_State *luaState);
            
            template<typename T, typename U>
            void defineInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction);

            template<typename B>
            void setInheritanceTable(lua_State *luaState);
            
            int callIndexMetamethod(lua_State *luaState);
            
            void pushInheritanceIndexMetaTable(lua_State *luaState);
            
            //--
            
            template<typename T>
            inline bool checkClassMetatableExistence(lua_State *luaState) {
                return checkClassMetatableExistence(luaState, std::type_index(typeid(UserDataWrapper<T>)));
            }
            
            template<typename T>
            UserDataWrapper<T> * getUserDataWrapper(lua_State *luaState, int index) {
                void * userData = lua_touserdata(luaState, index);
                if (userData != nullptr) {
                    if (lua_getmetatable(luaState, index) != 0) {
                        //stack: metatable
                        if (checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<T>))) == true) {
                            lua_pop(luaState, 1);
                            return static_cast<UserDataWrapper<T> *>(userData);
                        }
                        lua_pop(luaState, 1);
                    }
                }
                return nullptr;
            }

            template<typename T>
            void pushRootMetatableFromTypeHashTable(lua_State *luaState, const std::type_index &typeIndex) {
                // stack: typeFunctionHashTable
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: typeFunctionHashTable | type_index_udata*
                lua_newtable(luaState);
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable*
                lua_pushstring(luaState, "__index");
                lua_pushvalue(luaState, -2); // duplicates the metatable
                lua_rawset(luaState, -3); // metatable.__index = metatable
                basic::setLuaFunction(luaState, "__gc", [](lua_State *luaState) -> int {
                    static_cast<UserDataWrapper<T> *>(lua_touserdata(luaState, 1))->~UserDataWrapper<T>();
                    return 0;
                }, 0);
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable*
                lua_pushstring(luaState, gkTypeIndexKey);
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable* | gkTypeIndexKey
                lua_pushvalue(luaState, -3);
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable* | gkTypeIndexKey | type_index_udata*
                lua_rawset(luaState, -3); // used for getUserDataWrapper
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable*
                lua_pushvalue(luaState, -1);
                // stack: typeFunctionHashTable | type_index_udata* | rootMetatable* | rootMetatable*
                lua_insert(luaState, -4);
                // stack: rootMetatable* | typeFunctionHashTable | type_index_udata* | rootMetatable*
                lua_rawset(luaState, -3);
                lua_pop(luaState, 1);
                // stack: rootMetatable
                setUserDataWrapperBaseTable<T>(luaState);
                // stack: rootMetatable
                // Underlying type pointer conversion function
                basic::setLuaFunction(luaState, gkUnderlyingTypeFunctionKey, [](lua_State *luaState) -> int {
                    lua_pushlightuserdata(luaState, static_cast<void *>(static_cast<T *>(static_cast<UserDataWrapper<T> *>(lua_touserdata(luaState, 1)))));
                    return 1;
                }, 0);
                // stack: rootMetatable
            }
            
            template<typename T>
            void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, size_t typeHash) {
                // stack: typeManager
                lua_pushinteger(luaState, typeHash);
                lua_newtable(luaState);
                lua_pushvalue(luaState, -1);
                // stack: typeManager | typeHash | typeFunctionHashTable* | typeFunctionHashTable*
                lua_insert(luaState, -4);
                // stack: typeFunctionHashTable* | typeManager | typeHash | typeFunctionHashTable*
                lua_rawset(luaState, -3);
                // stack: typeFunctionHashTable | typeManager
                lua_pop(luaState, 1);
                // stack: typeFunctionHashTable
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
                        // stack: typeManager | typeFunctionHashTable
                        lua_remove(luaState, -2);
                        // stack: typeFunctionHashTable
                        lua_pushnil(luaState);
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(luaL_testudata(luaState, -2, gkTypeIndexMetatableName));
                            // stack: typeFunctionHashTable | type_index_udata | rootMetatable
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
                            // stack: typeFunctionHashTable | type_index_udata | rootMetatable
                        }
                        // stack: typeFunctionHashTable
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
                    lua_pushstring(luaState, gkTypeManagerVersionKey);
                    // stack: typeManager | gkTypeManagerVersionKey
                    lua_pushunsigned(luaState, TYPE_MANAGER_VERSION);
                    // stack: typeManager | gkTypeManagerVersionKey | TYPE_MANAGER_VERSION
                    lua_rawset(luaState, -3);
                    // stack: typeManager
                    pushRootMetatableFromTypeManager<T>(luaState, typeIndex, typeHash);
                    // stack: rootMetatable
                }
                return true;
            }
            
            template<typename T>
            inline void pushTypeIndexUserData(lua_State *luaState) {
                pushTypeIndexUserData(luaState, typeid(T));
            }
            
            // metatable[gkTypeFunctionsKey] = {[typeHash] = {[type_index_udata] = typeFunction}}
            // metatable[gkTypeFunctionsKey] = {[typeHash] = typeFunctionHashTable}
            // metatable[gkTypeFunctionsKey] = typeFunctionTable
            template<typename D, typename B>
            void setTypeFunction(lua_State *luaState) {
                std::type_index typeIndex = typeid(B);
                // stack: metatable
                pushTypeFunctionHashTable(luaState, typeIndex);
                // stack: metatable | typeFunctionHashTable
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: metatable | typeFunctionHashTable| type_index_udata*
                lua_pushcclosure(luaState, [](lua_State *luaState) -> int {
                    // no need for exception checking. Possible exceptions thrown by conversion function will be caught in [Lua]FunctionWrapperCaller. Type functions are only called by exchanger.
                    if (lua_islightuserdata(luaState, 1) == false) {
                        throw std::runtime_error("conversion function expected underlying lightuserdata");
                    }
                    lua_pushlightuserdata(luaState, static_cast<void *>(static_cast<B *>(static_cast<D *>(lua_touserdata(luaState, 1)))));
                    return 1;
                }, 0);
                // stack: metatable | typeFunctionHashTable | type_index_udata* | function*
                lua_rawset(luaState, -3);
                lua_pop(luaState, 1);
                // stack: metatable
            }
            
            template<typename T, typename U>
            void setTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
                std::type_index typeIndex = typeid(U);
                // stack: metatable
                pushTypeFunctionHashTable(luaState, typeIndex);
                // stack: metatable | typeFunctionHashTable
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: metatable | typeFunctionHashTable| type_index_udata*
                basic::pushUserData<std::function<U *(T *)>>(luaState, typeFunction);
                basic::pushClassMetatable<std::function<U *(T *)>>(luaState);
                lua_setmetatable(luaState, -2);
                // stack: metatable | typeFunctionHashTable| type_index_udata* | typeFunction_udata
                lua_pushcclosure(luaState, [](lua_State *luaState) -> int {
                    // no need for exception checking. Possible exceptions thrown by conversion function will be caught in [Lua]FunctionWrapperCaller. Type functions are only called by exchanger.
                    if (lua_islightuserdata(luaState, 1) == false) {
                        throw std::runtime_error("custom conversion function expected underlying lightuserdata");
                    }
                    lua_pushlightuserdata(luaState, static_cast<void *>((*static_cast<std::function<U *(T *)> *>(lua_touserdata(luaState, lua_upvalueindex(1))))(static_cast<T *>(lua_touserdata(luaState, 1)))));
                    return 1;
                }, 1);
                // stack: metatable | typeFunctionHashTable | type_index_udata* | function*
                lua_rawset(luaState, -3);
                lua_pop(luaState, 1);
                // stack: metatable
            }
            
            template<typename D, typename B>
            void defineTypeFunction(lua_State *luaState) {
                static_assert(std::is_same<D, B>::value == false, "conversion to itself");
                if (checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<D>))) == true) {
                    setTypeFunction<D, B>(luaState);
                } else {
                    pushClassMetatable<D>(luaState);
                    setTypeFunction<D, B>(luaState);
                    lua_pop(luaState, 1);
                }
            }
            
            template<typename T, typename U>
            void defineTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
                static_assert(std::is_same<T, U>::value == false, "conversion to itself");
                if (checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<T>))) == true) {
                    setTypeFunction(luaState, typeFunction);
                } else {
                    pushClassMetatable<T>(luaState);
                    setTypeFunction(luaState, typeFunction);
                    lua_pop(luaState, 1);
                }
            }
            
            // metatable[gkUserDataWrapperBaseTableKey] = {userDataWrapperBaseTypeIndex, userDataWrapperBaseConversionFunction}
            // metatable[gkUserDataWrapperBaseTableKey] = userDataWrapperBaseTable
            // provides conversion: UserDataWrapper<T> -> UserDataWrapperBase
            template<typename T>
            void setUserDataWrapperBaseTable(lua_State *luaState) {
                // stack: metatable
                lua_pushstring(luaState, gkUserDataWrapperBaseTableKey);
                lua_createtable(luaState, 2, 0);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable*
                pushTypeIndexUserData<UserDataWrapperBase>(luaState);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable* | userDataWrapperBaseTypeIndex*
                lua_rawseti(luaState, -2, USERDATAWRAPPERBASE_TYPE_INDEX_INDEX);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable*
                lua_pushcclosure(luaState, [](lua_State *luaState) -> int {
                    lua_pushlightuserdata(luaState, static_cast<UserDataWrapperBase *>(static_cast<UserDataWrapper<T> *>(lua_touserdata(luaState, 1))));
                    return 1;
                }, 0);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable* | userDataWrapperBaseFunction*
                lua_rawseti(luaState, -2, USERDATAWRAPPERBASE_FUNCTION_INDEX);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable*
                lua_rawset(luaState, -3);
                // stack: metatable
            }
            
            template<typename T>
            T * getConvertibleType(lua_State *luaState, int index) {
                lua_pushvalue(luaState, index);
                // stack: userdata
                if (lua_getmetatable(luaState, -1) != 0) {
                    // stack: userdata | metatable
                    if (pushConvertibleOrInheritedType(luaState, -2, std::type_index(typeid(T)), false) == true) {
                        // stack: userdata | typeTLightUserData
                        T * lightUserData = static_cast<T *>(lua_touserdata(luaState, -1));
                        lua_pop(luaState, 2);
                        return lightUserData;
                    } else {
                        // stack: userdata
                        lua_pop(luaState, 1);
                    }
                } else {
                    // stack: userdata
                    lua_pop(luaState, 1);
                }
                return nullptr;
            }
            
            template<typename D, typename B>
            void setInheritance(lua_State *luaState) {
                static_assert(std::is_same<D, B>::value == false, "Inheritance to itself");
                // stack: metatable
                setInheritanceTable<B>(luaState);
                setTypeFunction<D, B>(luaState);
                pushInheritanceIndexMetaTable(luaState);
                lua_setmetatable(luaState, -2);
                // stack: metatable
            }
            
            template<typename T, typename U>
            void setInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
                static_assert(std::is_same<T, U>::value == false, "Inheritance to itself");
                // stack: metatable
                setInheritanceTable<U>(luaState);
                setTypeFunction<T, U>(luaState, typeFunction);
                pushInheritanceIndexMetaTable(luaState);
                lua_setmetatable(luaState, -2);
                // stack: metatable
            }
            
            template<typename D, typename B>
            void defineInheritance(lua_State *luaState) {
                static_assert(std::is_same<D, B>::value == false, "Inheritance to itself");
                if (checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<D>))) == true) {
                    setInheritance<D, B>(luaState);
                } else {
                    pushClassMetatable<D>(luaState);
                    setInheritance<D, B>(luaState);
                    lua_pop(luaState, 1);
                }
            }
            
            template<typename T, typename U>
            void defineInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
                static_assert(std::is_same<T, U>::value == false, "Inheritance to itself");
                if (checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<T>))) == true) {
                    setInheritance(luaState, typeFunction);
                } else {
                    pushClassMetatable<T>(luaState);
                    setInheritance(luaState, typeFunction);
                    lua_pop(luaState, 1);
                }
            }
            
            // metatable[gkInheritanceKey] = {[number] = {baseTypeIndex, baseMetatable}}
            // metatable[gkInheritanceKey] = {[number] = baseTable}
            // metatable[gkInheritanceKey] = inheritanceTable
            template<typename B>
            void setInheritanceTable(lua_State *luaState) {
                // stack: metatable
                lua_pushstring(luaState, gkInheritanceKey);
                lua_rawget(luaState, -2);
                if (lua_istable(luaState, -1) == 0) {
                    // stack: metatable | nil
                    lua_pop(luaState, 1);
                    lua_newtable(luaState);
                    // stack: metatable | inheritanceTable*
                    lua_pushstring(luaState, gkInheritanceKey);
                    // stack: metatable | inheritanceTable* | gkInheritanceKey
                    lua_pushvalue(luaState, -2);
                    // stack: metatable | inheritanceTable* | gkInheritanceKey | inheritanceTable*
                    lua_rawset(luaState, -4);
                }
                // stack: metatable | inheritanceTable
                lua_createtable(luaState, 2, 0);
                // stack: metatable | inheritanceTable | baseTable*
                lua_pushvalue(luaState, -1);
                // stack: metatable | inheritanceTable | baseTable* | baseTable*
                lua_rawseti(luaState, -3, static_cast<int>(lua_rawlen(luaState, -3)) + 1);
                // stack: metatable | inheritanceTable | baseTable
                pushTypeIndexUserData<B>(luaState);
                // stack: metatable | inheritanceTable | baseTable | baseTypeIndex*
                lua_rawseti(luaState, -2, INHERITANCE_TYPE_INDEX_INDEX);
                // stack: metatable | inheritanceTable | baseTable
                pushClassMetatable<B>(luaState);
                // stack: metatable | inheritanceTable | baseTable | baseMetatable*
                lua_rawseti(luaState, -2, INHERITANCE_METATABLE_INDEX);
                // stack: metatable | inheritanceTable | baseTable
                lua_pop(luaState, 2);
                // stack: metatable
            }
        }
    }
}

#endif

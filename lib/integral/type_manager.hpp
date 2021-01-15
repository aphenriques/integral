//
//  type_manager.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2017, 2019, 2020, 2021 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_type_manager_hpp
#define integral_type_manager_hpp

#include <cstddef>
#include <functional>
#include <string>
#include <typeindex>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "basic.hpp"
#include "ConversionFunctionTraits.hpp"
#include "FunctionTraits.hpp"
#include "lua_compatibility.hpp"
#include "UnexpectedStackException.hpp"
#include "UserDataWrapper.hpp"

// typeid(T).name() cannot be used as an automatic indentifier for metatables. It is not safe: different classes might have the same typeid(T).name() (c++ standard)
// std::type_index is used as an automatic indentifier for metatables. Supposedly, it is safe.

// TypeManager organization (hashmap):
// REGISTRY[gkTypeManagerRegistryKey] = {[typeHash] = {[type_index_udata] = rootMetatable}}
// REGISTRY[gkTypeManagerRegistryKey] = {[typeHash] = typeHashBucket}
// REGISTRY[gkTypeManagerRegistryKey] = typeManager

// TypeManager (integral) version control:
// REGISTRY[gkTypeManagerRegistryKey][gkTypeManagerVersionKey] = TYPE_MANAGER_VERSION

// there might be clashes with typeHash. It is NOT a problem. TypeManager is implemented as a hashmap.

// Attention! The stored type_index (type_index_udata) is from std::type_index(typeid(UserDataWrapper<T>))
// This is useful when multiple integral versions are used.
// Maybe UserDataWrapper<T> is incompatible from different integral versions used together; this way, it will fail gracefully.

namespace integral {
    namespace detail {
        namespace type_manager {
            extern const char * const gkTypeManagerRegistryKey;
            extern const char * const gkTypeIndexKey;
            extern const char * const gkTypeManagerVersionKey;
            extern const char * const gkTypeManagerVersion;
            extern const char * const gkTypeIndexMetatableName;
            extern const char * const gkTypeFunctionsKey;
            extern const char * const gkUserDataWrapperBaseTableKey;
            extern const char * const gkUnderlyingTypeFunctionKey;
            extern const char * const gkInheritanceSearchTagKey;
            extern const char * const gkInheritanceKey;
            extern const char * const gkInheritanceIndexMetamethodKey;

            enum class InheritanceTable : int {
                kTypeIndexIndex = 1,
                kMetatableIndex = 2
            };

            enum class UserDataWrapperBaseTable : int {
                kTypeIndexIndex = 1,
                kFunctionIndex = 2
            };

            // stack argument: metatable
            const std::type_index * getClassMetatableType(lua_State *luaState);

            // stack argument: metatable
            bool checkClassMetatableType(lua_State *luaState, const std::type_index &typeIndex);

            // typeIndex is the class metatable type (typeIndex = std::type_index(typeid(UserDataWrapper<T>)))
            bool checkClassMetatableExistence(lua_State *luaState, const std::type_index &typeIndex);

            // T is the underlying type
            template<typename T>
            inline bool checkClassMetatableExistence(lua_State *luaState);

            template<typename T>
            UserDataWrapper<T> * getUserDataWrapper(lua_State *luaState, int index);

            template<typename T>
            void pushRootMetatableFromTypeHashBucketTable(lua_State *luaState, const std::type_index &typeIndex);

            // stack argument: typeManager
            // returns rootMetatable
            // pops typemanager from stack
            template<typename T>
            void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, std::size_t typeHash);

            template<typename T>
            bool pushClassMetatable(lua_State *luaState);

            void pushTypeIndexUserData(lua_State *luaState, const std::type_index &typeIndex);

            template<typename T>
            inline void pushTypeIndexUserData(lua_State *luaState);

            void setTypeFunctionHashTable(lua_State *luaState, std::size_t baseTypeHash);

            // stack argument: metatable
            // this function should be used only for the creation of typeFunctionHashTable
            void pushTypeFunctionHashTable(lua_State *luaState, const std::type_index &baseTypeIndex);

            // stack argument: metatable
            template<typename D, typename B>
            void setTypeFunction(lua_State *luaState);

            // stack argument: metatable
            template<typename F>
            void setTypeFunction(lua_State *luaState, F &&typeFunction);

            template<typename D, typename B>
            void defineTypeFunction(lua_State *luaState);

            template<typename F>
            void defineTypeFunction(lua_State *luaState, F &&typeFunction);

            // protects from recursion in a very unlikely scenario that could happen with 'synthetic' inheritance:
            // derived -inheritance-> base -synthetic-inheritance-> derived
            // index: metatable to be tagged
            bool checkInheritanceSearchTag(lua_State *luaState, int index);

            // protects from recursion in a very unlikely scenario that could happen with 'synthetic' inheritance:
            // derived -inheritance-> base -synthetic-inheritance-> derived
            // index: metatable to be tagged
            void tagInheritanceSearch(lua_State *luaState, int index);

            // protects from recursion in a very unlikely scenario that could happen with 'synthetic' inheritance:
            // derived -inheritance-> base -synthetic-inheritance-> derived
            // index: metatable to be tagged
            void untagInheritanceSearch(lua_State *luaState, int index);

            // index: userdata or lightuserdata of the underlying type of a userdata
            // stack argument: metatable
            // returns true if the convertible type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushDirectConvertibleType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex);

            // index: userdata (recursion = false) or lightuserdata (recursion = true) of the underlying type of a userdata stack index
            // stack argument: metatable
            // returns true if the convertible type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushConvertibleOrInheritedType(lua_State *luaState, int index, const std::type_index &convertibleTypeIndex, bool isRecursion);

            // stack argument: metatable
            template<typename T>
            void setUserDataWrapperBaseTable(lua_State *luaState);

            UserDataWrapperBase * getUserDataWrapperBase(lua_State *luaState, int index);

            // stack argument: metatable
            // returns true if the underlying type is pushed onto the stack, and false otherwise
            // metatable will be popped from stack in either case
            bool pushUnderlyingTypeLightUserData(lua_State *luaState, int index);

            template<typename T>
            T * getConvertibleType(lua_State *luaState, int index);

            // stack argument: metatable
            template<typename D, typename B>
            void setInheritance(lua_State *luaState);

            // stack argument: metatable
            template<typename F>
            void setInheritance(lua_State *luaState, F &&typeFunction);

            template<typename D, typename B>
            void defineInheritance(lua_State *luaState);

            template<typename F>
            void defineInheritance(lua_State *luaState, F &&typeFunction);

            // stack argument: inheritanceTable
            // returns true if inheritanceTable has typeIndex
            bool checkInheritanceTable(lua_State *luaState, const std::type_index &typeIndex);

            // stack argument: metatable
            template<typename B>
            void setInheritanceTable(lua_State *luaState);

            int callInheritanceIndexMetamethod(lua_State *luaState);

            void pushInheritanceIndexMetamethod(lua_State *luaState);

            // stack argument: metatable
            void setInheritanceIndexMetatable(lua_State *luaState);

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
            void pushRootMetatableFromTypeHashBucketTable(lua_State *luaState, const std::type_index &typeIndex) {
                // stack: typeHashBucket
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: typeHashBucket | type_index_udata*
                lua_newtable(luaState);
                // stack: typeHashBucket | type_index_udata* | rootMetatable*
                lua_pushstring(luaState, "__index");
                // stack: typeHashBucket | type_index_udata* | rootMetatable* | "__index"
                lua_pushvalue(luaState, -2); // duplicates the metatable
                // stack: typeHashBucket | type_index_udata* | rootMetatable* | "__index" | rootMetatable*
                lua_rawset(luaState, -3); // metatable.__index = metatable
                // stack: typeHashBucket | type_index_udata* | rootMetatable*
                basic::setLuaFunction(luaState, "__gc", [](lua_State *lambdaLuaState) -> int {
                    static_cast<UserDataWrapper<T> *>(lua_touserdata(lambdaLuaState, 1))->~UserDataWrapper<T>();
                    return 0;
                }, 0);
                // stack: typeHashBucket | type_index_udata* | rootMetatable*
                lua_pushstring(luaState, gkTypeIndexKey);
                // stack: typeHashBucket | type_index_udata* | rootMetatable* | gkTypeIndexKey
                lua_pushvalue(luaState, -3);
                // stack: typeHashBucket | type_index_udata* | rootMetatable* | gkTypeIndexKey | type_index_udata*
                lua_rawset(luaState, -3); // used for getUserDataWrapper
                // stack: typeHashBucket | type_index_udata* | rootMetatable*
                lua_pushvalue(luaState, -1);
                // stack: typeHashBucket | type_index_udata* | rootMetatable* | rootMetatable*
                lua_insert(luaState, -4);
                // stack: rootMetatable* | typeHashBucket | type_index_udata* | rootMetatable*
                lua_rawset(luaState, -3);
                // stack: rootMetatable | typeHashBucket
                lua_pop(luaState, 1);
                // stack: rootMetatable
                setUserDataWrapperBaseTable<T>(luaState);
                // stack: rootMetatable
                // Underlying type pointer conversion function
                basic::setLuaFunction(luaState, gkUnderlyingTypeFunctionKey, [](lua_State *lambdaLuaState) -> int {
                    lua_pushlightuserdata(lambdaLuaState, static_cast<void *>(static_cast<T *>(static_cast<UserDataWrapper<T> *>(lua_touserdata(lambdaLuaState, 1)))));
                    return 1;
                }, 0);
                // stack: rootMetatable
            }

            template<typename T>
            void pushRootMetatableFromTypeManager(lua_State *luaState, const std::type_index &typeIndex, std::size_t typeHash) {
                // stack: typeManager
                static_assert(sizeof(typeHash) <= sizeof(lua_Integer), "lua_Integer cannot accommodate typeHash");
                lua_pushinteger(luaState, static_cast<lua_Integer>(typeHash));
                // stack: typeManager | typeHash
                lua_newtable(luaState);
                // stack: typeManager | typeHash | typeHashBucket*
                lua_pushvalue(luaState, -1);
                // stack: typeManager | typeHash | typeHashBucket* | typeHashBucket*
                lua_insert(luaState, -4);
                // stack: typeHashBucket* | typeManager | typeHash | typeHashBucket*
                lua_rawset(luaState, -3);
                // stack: typeHashBucket | typeManager
                lua_pop(luaState, 1);
                // stack: typeHashBucket
                pushRootMetatableFromTypeHashBucketTable<T>(luaState, typeIndex);
                // stack: rootMetatable
            }

            template<typename T>
            bool pushClassMetatable(lua_State *luaState) {
                // Attention! The stored type_index is UserDataWrapper<T>
                // This is useful when multiple integral versions are used.
                // Maybe UserDataWrapper<T> is incompatible from different integral versions used together; this way, it will fail gracefully.
                static_assert(std::is_same_v<std::decay_t<T>, std::string> == false, "cannot push std::string metatable. integral treats it as a primitive lua type");
                const std::type_index typeIndex = typeid(UserDataWrapper<T>);
                const std::size_t typeHash = typeIndex.hash_code();
                lua_pushstring(luaState, gkTypeManagerRegistryKey);
                // stack: gkTypeManagerRegistryKey
                lua_rawget(luaState, LUA_REGISTRYINDEX);
                // stack: typeManager (?)
                if (lua_istable(luaState, -1) != 0) {
                    // stack: typeManager
                    static_assert(sizeof(typeHash) <= sizeof(lua_Integer), "lua_Integer cannot accommodate typeHash");
                    lua_pushinteger(luaState, static_cast<lua_Integer>(typeHash));
                    // stack: typeManager | typeHash
                    lua_rawget(luaState, -2);
                    // stack: typeManager | typeHashBucket (?)
                    if (lua_istable(luaState, -1) != 0) {
                        // stack: typeManager | typeHashBucket
                        lua_remove(luaState, -2);
                        // stack: typeHashBucket
                        lua_pushnil(luaState);
                        // stack: typeHashBucket | nil
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 1), hasNext = lua_next(luaState, -2)) {
                            std::type_index *storedTypeIndex = static_cast<std::type_index *>(lua_compatibility::testudata(luaState, -2, gkTypeIndexMetatableName));
                            // stack: typeHashBucket | type_index_udata (?) | rootMetatable (?)
                            if (storedTypeIndex != nullptr) {
                                // stack: typeHashBucket | type_index_udata | rootMetatable (?)
                                if (*storedTypeIndex == typeIndex) {
                                    lua_remove(luaState, -2);
                                    // stack: typeHashBucket | rootMetatable (?)
                                    if (lua_istable(luaState, -1) != 0) {
                                        // stack: typeHashBucket | rootMetatable
                                        lua_remove(luaState, -2);
                                        // stack: rootMetatable
                                        return false;
                                    } else {
                                        // stack: typeHashBucket | ?
                                        throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeManager: expected rootMetatable at index -1");
                                    }
                                }
                            } else {
                                // stack: typeHashBucket | ? | rootMetatable (?)
                                throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "corrupted TypeManager: expected type_index_udata at index -2");
                            }
                            // stack: typeHashBucket | type_index_udata | rootMetatable (?)
                        }
                        // stack: typeHashBucket
                        pushRootMetatableFromTypeHashBucketTable<T>(luaState, typeIndex);
                        // stack: rootMetatable
                    } else {
                        // stack: typeManager | nil (?)
                        lua_pop(luaState, 1);
                        // stack: typeManager
                        pushRootMetatableFromTypeManager<T>(luaState, typeIndex, typeHash);
                        // stack: rootMetatable
                    }
                } else {
                    // stack: nil (?)
                    lua_pop(luaState, 1);
                    // stack:
                    lua_newtable(luaState);
                    // stack: typeManager*
                    lua_pushstring(luaState, gkTypeManagerRegistryKey);
                    // stack: typeManager* | gkTypeManagerRegistryKey
                    lua_pushvalue(luaState, -2);
                    // stack: typeManager* | gkTypeManagerRegistryKey | typeManager*
                    lua_rawset(luaState, LUA_REGISTRYINDEX);
                    // stack: typeManager
                    lua_pushstring(luaState, gkTypeManagerVersionKey);
                    // stack: typeManager | gkTypeManagerVersionKey
                    lua_pushstring(luaState, gkTypeManagerVersion);
                    // stack: typeManager | gkTypeManagerVersionKey | gkTypeManagerVersion
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
            // metatable[gkTypeFunctionsKey] = {[typeHash] = typeHashBucket}
            // metatable[gkTypeFunctionsKey] = typeFunctionTable
            template<typename D, typename B>
            void setTypeFunction(lua_State *luaState) {
                static_assert(std::is_same_v<std::remove_cv_t<D>, D> == true, "D is cv qualified");
                static_assert(std::is_same_v<std::remove_cv_t<B>, B> == true, "B is cv qualified");
                static_assert(std::is_same_v<D, B> == false, "conversion to itself");
                static_assert(std::is_base_of_v<B, D> == true, "D must be derived from B");
                std::type_index typeIndex = typeid(B);
                // stack: metatable
                pushTypeFunctionHashTable(luaState, typeIndex);
                // stack: metatable | typeHashBucket
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: metatable | typeHashBucket | type_index_udata*
                lua_pushcclosure(luaState, [](lua_State *lambdaLuaState) -> int {
                    if (lua_islightuserdata(lambdaLuaState, 1) != 0) {
                        lua_pushlightuserdata(lambdaLuaState, static_cast<void *>(static_cast<B *>(static_cast<D *>(lua_touserdata(lambdaLuaState, 1)))));
                        return 1;
                    } else {
                        throw UnexpectedStackException(lambdaLuaState, __FILE__, __LINE__, __func__, "conversion function expected underlying lightuserdata");
                    }
                }, 0);
                // stack: metatable | typeHashBucket | type_index_udata* | function*
                lua_rawset(luaState, -3);
                lua_pop(luaState, 1);
                // stack: metatable
            }

            template<typename F>
            void setTypeFunction(lua_State *luaState, F &&typeFunction) {
                using ConversionFunctionTraits = ConversionFunctionTraits<typename FunctionTraits<F>::Signature>;
                using OriginalType = typename ConversionFunctionTraits::OriginalType;
                using ConversionType = typename ConversionFunctionTraits::ConversionType;
                static_assert(std::is_same_v<std::remove_cv_t<ConversionType>, ConversionType> == true, "ConversionType is cv qualified");
                static_assert(std::is_same_v<std::remove_cv_t<OriginalType>, ConversionType> == false, "conversion to itself");
                std::type_index typeIndex = typeid(ConversionType);
                // stack: metatable
                pushTypeFunctionHashTable(luaState, typeIndex);
                // stack: metatable | typeHashBucket
                pushTypeIndexUserData(luaState, typeIndex);
                // stack: metatable | typeHashBucket | type_index_udata*
                basic::pushUserData<std::function<ConversionType *(OriginalType *)>>(luaState, std::forward<F>(typeFunction));
                // stack: metatable | typeHashBucket | type_index_udata* | typeFunction_udata_no_metatable
                basic::pushClassMetatable<std::function<ConversionType *(OriginalType *)>>(luaState);
                // stack: metatable | typeHashBucket | type_index_udata* | typeFunction_udata_no_metatable | typeFunction_udata_metatable
                lua_setmetatable(luaState, -2);
                // stack: metatable | typeHashBucket | type_index_udata* | typeFunction_udata
                lua_pushcclosure(luaState, [](lua_State *lambdaLuaState) -> int {
                    // no need for exception checking. Possible exceptions thrown by conversion function will be caught in [Lua]FunctionWrapperCaller. Type functions are only called by exchanger.
                    if (lua_islightuserdata(lambdaLuaState, 1) != 0) {
                        lua_pushlightuserdata(lambdaLuaState, static_cast<void *>((*static_cast<std::function<ConversionType *(OriginalType *)> *>(lua_touserdata(lambdaLuaState, lua_upvalueindex(1))))(static_cast<OriginalType *>(lua_touserdata(lambdaLuaState, 1)))));
                        return 1;
                    } else {
                        throw UnexpectedStackException(lambdaLuaState, __FILE__, __LINE__, __func__, "custom conversion function expected underlying lightuserdata");
                    }
                }, 1);
                // stack: metatable | typeHashBucket | type_index_udata* | function*
                lua_rawset(luaState, -3);
                // stack: metatable | typeHashBucket
                lua_pop(luaState, 1);
                // stack: metatable
            }

            template<typename D, typename B>
            void defineTypeFunction(lua_State *luaState) {
                if (lua_istable(luaState, -1) != 0 && checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<D>))) == true) {
                    setTypeFunction<D, B>(luaState);
                } else {
                    pushClassMetatable<D>(luaState);
                    setTypeFunction<D, B>(luaState);
                    lua_pop(luaState, 1);
                }
            }

            template<typename F>
            void defineTypeFunction(lua_State *luaState, F &&typeFunction) {
                using ConversionFunctionTraits = ConversionFunctionTraits<typename FunctionTraits<F>::Signature>;
                using OriginalType = typename ConversionFunctionTraits::OriginalType;
                if (lua_istable(luaState, -1) != 0 && checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<std::remove_cv_t<OriginalType>>))) == true) {
                    setTypeFunction(luaState, std::forward<F>(typeFunction));
                } else {
                    pushClassMetatable<std::remove_cv_t<OriginalType>>(luaState);
                    setTypeFunction(luaState, std::forward<F>(typeFunction));
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
                // stack: metatable | gkUserDataWrapperBaseTableKey
                lua_createtable(luaState, 2, 0);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable*
                pushTypeIndexUserData<UserDataWrapperBase>(luaState);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable* | userDataWrapperBaseTypeIndex*
                lua_rawseti(luaState, -2, static_cast<lua_Integer>(UserDataWrapperBaseTable::kTypeIndexIndex));
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable*
                lua_pushcclosure(luaState, [](lua_State *lambdaLuaState) -> int {
                    lua_pushlightuserdata(lambdaLuaState, static_cast<UserDataWrapperBase *>(static_cast<UserDataWrapper<T> *>(lua_touserdata(lambdaLuaState, 1))));
                    return 1;
                }, 0);
                // stack: metatable | gkUserDataWrapperBaseTableKey | userDataWrapperBaseTable* | userDataWrapperBaseFunction*
                lua_rawseti(luaState, -2, static_cast<lua_Integer>(UserDataWrapperBaseTable::kFunctionIndex));
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
                        // stack:
                        return lightUserData;
                    } else {
                        // stack: userdata
                        lua_pop(luaState, 1);
                        // stack:
                    }
                } else {
                    // stack: userdata
                    lua_pop(luaState, 1);
                    // stack:
                }
                return nullptr;
            }

            template<typename D, typename B>
            void setInheritance(lua_State *luaState) {
                static_assert(std::is_same_v<std::remove_cv_t<D>, D> == true, "D is cv qualified");
                static_assert(std::is_same_v<std::remove_cv_t<B>, B> == true, "B is cv qualified");
                static_assert(std::is_same_v<D, B> == false, "inheritance to itself");
                static_assert(std::is_base_of_v<B, D> == true, "D must be derived from B");
                // stack: metatable
                setInheritanceTable<B>(luaState);
                setTypeFunction<D, B>(luaState);
                // preserves a previously defined metatable (possibly with other metamethods)
                setInheritanceIndexMetatable(luaState);
                // stack: metatable
            }

            template<typename F>
            void setInheritance(lua_State *luaState, F &&typeFunction) {
                using ConversionFunctionTraits = ConversionFunctionTraits<typename FunctionTraits<F>::Signature>;
                using ConversionType = typename ConversionFunctionTraits::ConversionType;
                static_assert(std::is_same_v<std::remove_cv_t<typename ConversionFunctionTraits::OriginalType>, ConversionType> == false, "inheritance to itself");
                static_assert(std::is_same_v<std::remove_cv_t<ConversionType>, ConversionType> == true, "ConversionType is cv qualified");
                // stack: metatable
                setInheritanceTable<ConversionType>(luaState);
                setTypeFunction(luaState, std::forward<F>(typeFunction));
                // preserves a previously defined metatable (possibly with other metamethods)
                setInheritanceIndexMetatable(luaState);
                // stack: metatable
            }

            template<typename D, typename B>
            void defineInheritance(lua_State *luaState) {
                if (lua_istable(luaState, -1) != 0 && checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<D>))) == true) {
                    setInheritance<D, B>(luaState);
                } else {
                    pushClassMetatable<D>(luaState);
                    setInheritance<D, B>(luaState);
                    lua_pop(luaState, 1);
                }
            }

            template<typename F>
            void defineInheritance(lua_State *luaState, F &&typeFunction) {
                using ConversionFunctionTraits = ConversionFunctionTraits<typename FunctionTraits<F>::Signature>;
                using OriginalType = typename ConversionFunctionTraits::OriginalType;
                if (lua_istable(luaState, -1) != 0 && checkClassMetatableType(luaState, std::type_index(typeid(UserDataWrapper<std::remove_cv_t<OriginalType>>))) == true) {
                    setInheritance(luaState, std::forward<F>(typeFunction));
                } else {
                    pushClassMetatable<std::remove_cv_t<OriginalType>>(luaState);
                    setInheritance(luaState, std::forward<F>(typeFunction));
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
                // stack: metatable | gkInheritanceKey
                lua_rawget(luaState, -2);
                // stack: metatable | inheritanceTable (?)
                if (lua_istable(luaState, -1) == 0) {
                    // stack: metatable | nil
                    lua_pop(luaState, 1);
                    // stack: metatable
                    lua_newtable(luaState);
                    // stack: metatable | inheritanceTable*
                    lua_pushstring(luaState, gkInheritanceKey);
                    // stack: metatable | inheritanceTable* | gkInheritanceKey
                    lua_pushvalue(luaState, -2);
                    // stack: metatable | inheritanceTable* | gkInheritanceKey | inheritanceTable*
                    lua_rawset(luaState, -4);
                    // stack: metatable | inheritanceTable
                } else if (checkInheritanceTable(luaState, std::type_index(typeid(B))) == true) {
                    // stack: metatable | inheritanceTable
                    throw exception::LogicException(__FILE__, __LINE__, __func__, "trying to set existing inheritance");
                }
                // stack: metatable | inheritanceTable
                lua_createtable(luaState, 2, 0);
                // stack: metatable | inheritanceTable | baseTable*
                lua_compatibility::pushunsigned(luaState, lua_compatibility::rawlen(luaState, -2) + 1);
                // stack: metatable | inheritanceTable | baseTable* | baseTableIndex
                lua_pushvalue(luaState, -2);
                // stack: metatable | inheritanceTable | baseTable* | baseTableIndex | baseTable*
                lua_rawset(luaState, -4);
                // stack: metatable | inheritanceTable | baseTable
                pushTypeIndexUserData<B>(luaState);
                // stack: metatable | inheritanceTable | baseTable | baseTypeIndex*
                lua_rawseti(luaState, -2, static_cast<lua_Integer>(InheritanceTable::kTypeIndexIndex));
                // stack: metatable | inheritanceTable | baseTable
                pushClassMetatable<B>(luaState);
                // stack: metatable | inheritanceTable | baseTable | baseMetatable*
                lua_rawseti(luaState, -2, static_cast<lua_Integer>(InheritanceTable::kMetatableIndex));
                // stack: metatable | inheritanceTable | baseTable
                lua_pop(luaState, 2);
                // stack: metatable
            }
        }
    }
}

#endif

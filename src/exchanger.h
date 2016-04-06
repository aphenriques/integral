//
//  exchanger.h
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016  André Pereira Henriques
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

#ifndef integral_exchanger_h
#define integral_exchanger_h

#include <array>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <lua.hpp>
#include "exception/Exception.h"
#include "Adaptor.h"
#include "ArgumentException.h"
#include "basic.h"
#include "generic.h"
#include "IsTemplateClass.h"
#include "lua_compatibility.h"
#include "LuaPack.h"
#include "type_manager.h"
#include "UserDataWrapper.h"
#include "UserDataWrapperBase.h"

namespace integral {
    namespace detail {
        namespace exchanger {
            template<typename T>
            using LuaVector = Adaptor<std::vector<T>>;
            
            template<typename T, std::size_t N>
            using LuaArray = Adaptor<std::array<T, N>>;
            
            template<typename T, typename U>
            using LuaUnorderedMap = Adaptor<std::unordered_map<T, U>>;
            
            template<typename ...T>
            using LuaTuple = Adaptor<std::tuple<T...>>;
            
            template<typename T>
            T & getObject(lua_State *luaState, int index);
            
            template<typename T, typename ...A>
            void pushObject(lua_State *luaState, A &&...arguments);
            
            template<typename T, typename Enable = void>
            class Exchanger {
            public:
                inline static T & get(lua_State *luaState, int index);
                
                template<typename ...A>
                inline static void push(lua_State *luaState, A &&...arguments);
            };
            
            template<typename T>
            class Exchanger<T *> {
            public:
                // Pointers are unsafe
                static T & get(lua_State *luaState, int index) = delete;
                static void push(lua_State *luaState, T *pointer) = delete;
            };
            
            template<>
            class Exchanger<const char *> {
            public:
                static const char * get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const char *string);
            };
            
            template<>
            class Exchanger<std::string> {
            public:
                static std::string get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const std::string &string);
            };
            
            template<typename T>
            class Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type> {
            public:
                static T get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, T number);
            };
            
            template<typename T>
            class Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type> {
            public:
                static T get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, T number);
            };
            
            template<>
            class Exchanger<bool> {
            public:
                static bool get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, bool boolean);
            };
            
            template<typename T>
            class Exchanger< T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
            public:
                static T get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, T number);
            };
            
            template<typename T>
            class Exchanger<LuaVector<T>> {
            public:
                static LuaVector<T> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const LuaVector<T> &luaVector);
            };
            
            template<typename T, std::size_t N>
            class Exchanger<LuaArray<T, N>> {
            public:
                static LuaArray<T, N> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const LuaArray<T, N> &luaArray);
            };
            
            template<typename T, typename U>
            class Exchanger<LuaUnorderedMap<T, U>> {
            public:
                static LuaUnorderedMap<T, U> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const LuaUnorderedMap<T, U> &unorderedMap);
            };
            
            template<typename ...T>
            class Exchanger<LuaTuple<T...>> {
            public:
                inline static LuaTuple<T...> get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const LuaTuple<T...> &tuple);
                inline static void push(lua_State *luaState, T &&...t);
                
            private:
                template<unsigned ...S>
                static LuaTuple<T...> get(lua_State *luaState, int index, std::integer_sequence<unsigned, S...>);
                
                template<unsigned I, typename U>
                static U getElementFromTable(lua_State *luaState, int index);
                
                template<unsigned ...S>
                static void push(lua_State *luaState, const LuaTuple<T...> &tuple, std::integer_sequence<unsigned, S...>);
                
                template<unsigned ...S>
                static void push(lua_State *luaState, T &&...t, std::integer_sequence<unsigned, S...>);
                
                template<unsigned I, typename U>
                static void setElementInTable(lua_State *luaState, U &&element, int index);
                
                template<unsigned I, typename U>
                static void setElementInTable(lua_State *luaState, const U &element, int index);
            };

            template<typename T>
            using ExchangerType = Exchanger<generic::BasicType<T>>;
            
            template<typename T>
            inline decltype(auto) singleGet(lua_State *luaState, int index);
            
            template<typename T, typename ...A>
            inline void singlePush(lua_State *luaState, A &&...arguments);
            
            template<typename T>
            inline decltype(auto) get(lua_State *luaState, int index);
            
            template<typename T, typename ...A>
            inline void push(lua_State *luaState, A &&...arguments);
            
            inline void pushCopy(lua_State *luaState);
            
            template<typename A, typename ...B>
            void pushCopy(lua_State *luaState, A &&firstArgument, B &&...remainingArguments);
            
            // Exchanger<LuaPack<T...>> is not like other Exchanger specializations because it can push and get more than 1 element from the lua stack, so it must be used with caution. LuaPack cannot be an element of LuaVector, LuaArray, LuaUnorderedMap or LuaTuple, because their Exchangers are designed to work with single elements. These adaptors use singleGet(...) and singlePush(...) to assert that LuaPack is not a type of one of their elements.
            template<typename ...T>
            class Exchanger<LuaPack<T...>> {
            public:
                inline static LuaPack<T...> get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const LuaPack<T...> &luaPack);
                static void push(lua_State *luaState, T &&...luaPack);
                
            private:
                template<unsigned ...S>
                static LuaPack<T...> get(lua_State *luaState, int index, std::integer_sequence<unsigned, S...>);
                
                template<unsigned ...S>
                inline static void push(lua_State *luaState, const LuaPack<T...> &luaPack, std::integer_sequence<unsigned, S...>);
                
                inline static void pushGeneric(lua_State *luaState);
                
                template<typename U, typename ...V>
                static void pushGeneric(lua_State *luaState, U &&value, V &&...remainingPack);
            };
            
            //--
            
            // dynamic_cast is faster then getConvertibleType, but getConvertibleType provides the expected behaviour with synthetic inheritance
            template<typename T>
            T & getObject(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) != 0) {
                    UserDataWrapper<T> *userDataWrapper = type_manager::getUserDataWrapper<T>(luaState, index);
                    if (userDataWrapper != nullptr) {
                        return *static_cast<T *>(userDataWrapper);
                    } else {
                        T *object = type_manager::getConvertibleType<T>(luaState, index);
                        if (object != nullptr) {
                            return *object;
                        } else {
                            UserDataWrapperBase *userDataWrapperBase = type_manager::getUserDataWrapperBase(luaState, index);
                            if (userDataWrapperBase != nullptr) {
                                T *castenObject = dynamic_cast<T *>(userDataWrapperBase);
                                if (castenObject != nullptr) {
                                    return *castenObject;
                                } else {
                                    throw ArgumentException(luaState, index, "invalid dynamic_cast - incompatible userdata type or ambiguous cast");
                                }
                            } else {
                                throw ArgumentException(luaState, index, "unknown userdata type or incompatible UserDataWrapperBase objects");
                            }
                        }
                    }
                } else {
                    throw ArgumentException::createTypeErrorException(luaState, index, "userdata");
                }
            }
            
            template<typename T, typename ...A>
            void pushObject(lua_State *luaState, A &&...arguments) {
                basic::pushUserData<UserDataWrapper<T>>(luaState, std::forward<A>(arguments)...);
                type_manager::pushClassMetatable<T>(luaState); // type_manager will automatically register unknown types
                lua_setmetatable(luaState, -2);
            }

            template<typename T, typename Enable>
            inline T & Exchanger<T, Enable>::get(lua_State *luaState, int index) {
                return getObject<T>(luaState, index);
            }
            
            template<typename T, typename Enable>
            template<typename ...A>
            inline void Exchanger<T, Enable>::push(lua_State *luaState, A &&...arguments) {
                pushObject<T>(luaState, std::forward<A>(arguments)...);
            }
            
            inline void Exchanger<const char *>::push(lua_State *luaState, const char *string) {
                lua_pushstring(luaState, string);
            }
            
            inline void Exchanger<std::string>::push(lua_State *luaState, const std::string &string) {
                lua_pushstring(luaState, string.c_str());
            }
            
            template<typename T>
            T Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    lua_Integer integer = lua_compatibility::tointegerx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(integer);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userData = type_manager::getConvertibleType<T>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                }
            }
            
            template<typename T>
            inline void Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type>::push(lua_State *luaState, T number) {
                lua_pushinteger(luaState, static_cast<lua_Integer>(number));
            }
            
            template<typename T>
            T Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    // "auto" keyword for lua compatibility
                    auto unsignedNumber = lua_compatibility::tounsignedx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(unsignedNumber);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userData = type_manager::getConvertibleType<T>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                }
            }
            
            template<typename T>
            inline void Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type>::push(lua_State *luaState, T number) {
                lua_compatibility::pushunsigned(luaState, number);
            }
            
            inline void Exchanger<bool>::push(lua_State *luaState, bool boolean) {
                lua_pushboolean(luaState, static_cast<int>(boolean));
            }
            
            template<typename T>
            T Exchanger< T, typename std::enable_if<std::is_floating_point<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    lua_Number number = lua_compatibility::tonumberx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(number);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userData = type_manager::getConvertibleType<T>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                }
            }
            
            template<typename T>
            inline void Exchanger< T, typename std::enable_if<std::is_floating_point<T>::value>::type>::push(lua_State *luaState, T number) {
                lua_pushnumber(luaState, static_cast<lua_Number>(number));
            }
            
            template<typename T>
            LuaVector<T> Exchanger<LuaVector<T>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, -1);
                        LuaVector<T> returnVector;
                        returnVector.reserve(tableSize);
                        for (std::size_t i = 1; i <= tableSize; ++i) {
                            // stack: table
                            lua_compatibility::pushunsigned(luaState, i);
                            // stack: table | i
                            lua_rawget(luaState, -2);
                            // stack: table | luaVectorElement (?)
                            try {
                                returnVector.push_back(singleGet<T>(luaState, -1));
                            } catch (const ArgumentException &argumentException) {
                                // stack: table | ?
                                lua_pop(luaState, 2);
                                throw ArgumentException(luaState, index, std::string("invalid table - LuaVector - element: " ) + argumentException.what());
                            }
                            // stack: table | luaVectorElement
                            lua_pop(luaState, 1);
                            // stack: table
                        }
                        // stack: table
                        lua_pop(luaState, 1);
                        return returnVector;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    LuaVector<T> *userData = type_manager::getConvertibleType<LuaVector<T>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or LuaVector");
                    }
                }
            }
            
            template<typename T>
            void Exchanger<LuaVector<T>>::push(lua_State *luaState, const LuaVector<T> &luaVector) {
                using SizeType = typename LuaVector<T>::size_type;
                const SizeType vectorSize = luaVector.size();
                if (vectorSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(vectorSize), 0);
                    // stack: table
                    for (SizeType i = 0; i < vectorSize; ++i) {
                        // stack: table
                        lua_compatibility::pushunsigned(luaState, i + 1);
                        // stack: table | i
                        singlePush<T>(luaState, luaVector.at(i));
                        // stack: table | i | luaVectorElement
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "LuaVector is too big");
                }
            }
            
            template<typename T, std::size_t N>
            LuaArray<T, N> Exchanger<LuaArray<T, N>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, -1);
                        if (tableSize == N) {
                            LuaArray<T, N> returnArray;
                            for (std::size_t i = 1; i <= tableSize; ++i) {
                                // stack: table
                                lua_compatibility::pushunsigned(luaState, i);
                                // stack: table | i
                                lua_rawget(luaState, -2);
                                // stack: table | luaArrayElement (?)
                                try {
                                    returnArray.at(i - 1) = singleGet<T>(luaState, -1);
                                } catch (const ArgumentException &argumentException) {
                                    // stack: table | ?
                                    lua_pop(luaState, 2);
                                    throw ArgumentException(luaState, index, std::string("invalid table - LuaArray - element: " ) + argumentException.what());
                                }
                                // stack: table | luaArrayElement
                                lua_pop(luaState, 1);
                                // stack: table
                            }
                            // stack: table
                            lua_pop(luaState, 1);
                            return returnArray;
                        } else {
                            std::stringstream errorMessage;
                            errorMessage << "wrong table - LuaArray - size: expected " << N << ", got " << tableSize;
                            throw ArgumentException(luaState, index, errorMessage.str());
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    LuaArray<T, N> *userData = type_manager::getConvertibleType<LuaArray<T, N>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or LuaArray");
                    }
                }
            }
            
            template<typename T, std::size_t N>
            void Exchanger<LuaArray<T, N>>::push(lua_State *luaState, const LuaArray<T, N> &luaArray) {
                if (N <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(N), 0);
                    // stack: table
                    for (std::size_t i = 0; i < N; ++i) {
                        // stack: table
                        lua_compatibility::pushunsigned(luaState, i + 1);
                        // stack: table | i
                        singlePush<T>(luaState, luaArray.at(i));
                        // stack: table | i | luaArrayElement
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "LuaArray is too big");
                }
            }
            
            template<typename T, typename U>
            LuaUnorderedMap<T, U> Exchanger<LuaUnorderedMap<T, U>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        LuaUnorderedMap<T, U> returnUnorderedMap;
                        lua_pushnil(luaState);
                        // atention! the key is pushed again on the stack to preserve its type (so 2 values are popped in each iteration)
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 2), hasNext = lua_next(luaState, -2)) {
                            // stack: table | key (?) | value (?)
                            // stack types can be changed by integral::get (for instance, a number type is converted to string in place in the stack by calling lua_tostring). This corrupts the table traversal. To avoid this, the key value is copied.
                            lua_pushvalue(luaState, -2);
                            // stack: table | key (?) | value (?) | key (?)
                            try {
                                returnUnorderedMap.emplace(singleGet<T>(luaState, -1), singleGet<U>(luaState, -2));
                            } catch (const ArgumentException &argumentException) {
                                // stack: table | ? | ? | ?
                                lua_pop(luaState, 4);
                                throw ArgumentException(luaState, index, std::string("invalid table - LuaUnorderedMap: " ) + argumentException.what());
                            }
                        }
                        // stack: table
                        lua_pop(luaState, 1);
                        return returnUnorderedMap;

                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    LuaUnorderedMap<T, U> *userData = type_manager::getConvertibleType<LuaUnorderedMap<T, U>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or LuaUnorderedMap");
                    }
                }
            }
            
            template<typename T, typename U>
            void Exchanger<LuaUnorderedMap<T, U>>::push(lua_State *luaState, const LuaUnorderedMap<T, U> &luaUnorderedMap) {
                using SizeType = typename LuaUnorderedMap<T, U>::size_type;
                const SizeType unorderedMapSize = luaUnorderedMap.size();
                if (unorderedMapSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(unorderedMapSize), 0);
                    // stack: table
                    for (const auto& keyValue : luaUnorderedMap) {
                        // stack: table
                        singlePush<T>(luaState, keyValue.first);
                        // stack: table | key
                        singlePush<U>(luaState, keyValue.second);
                        // stack: table | key | value
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "LuaUnorderedMap is too big");
                }
            }
            
            template<typename ...T>
            inline LuaTuple<T...> Exchanger<LuaTuple<T...>>::get(lua_State *luaState, int index) {
                return Exchanger<LuaTuple<T...>>::get(luaState, index, std::make_integer_sequence<unsigned, sizeof...(T)>());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaTuple<T...>>::push(lua_State *luaState, const LuaTuple<T...> &tuple) {
                Exchanger<LuaTuple<T...>>::push(luaState, tuple, std::make_integer_sequence<unsigned, sizeof...(T)>());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaTuple<T...>>::push(lua_State *luaState, T &&...t) {
                Exchanger<LuaTuple<T...>>::push(luaState, std::forward<T>(t)..., std::make_integer_sequence<unsigned, sizeof...(T)>());
            }
            
            template<typename ...T>
            template<unsigned ...S>
            LuaTuple<T...> Exchanger<LuaTuple<T...>>::get(lua_State *luaState, int index, std::integer_sequence<unsigned, S...>) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, index);
                        constexpr unsigned keTupleSize = sizeof...(T);
                        if (tableSize == keTupleSize) {
                            try {
                                return LuaTuple<T...>(getElementFromTable<S + 1, T>(luaState, index)...);
                            } catch (const ArgumentException &argumentException) {
                                throw ArgumentException(luaState, index, std::string("invalid table - LuaTuple: " ) + argumentException.what());
                            }
                        } else {
                            std::stringstream errorMessage;
                            errorMessage << "wrong table - LuaTuple - size: expected " <<  keTupleSize << ", got " << tableSize;
                            throw ArgumentException(luaState, index, errorMessage.str());
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    LuaTuple<T...> *userData = type_manager::getConvertibleType<LuaTuple<T...>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or LuaTuple");
                    }
                }
            }
            
            template<typename ...T>
            template<unsigned I, typename U>
            U Exchanger<LuaTuple<T...>>::getElementFromTable(lua_State *luaState, int index) {
                if (lua_istable(luaState, index) != 0) {
                    lua_pushvalue(luaState, index);
                    // stack: table
                    lua_compatibility::pushunsigned(luaState, I);
                    // stack: table | i
                    lua_rawget(luaState, -2);
                    // stack: table | luaArrayElement (?)
                    auto returnElement = singleGet<U>(luaState, -1);
                    // stack: table | luaArrayElement
                    lua_pop(luaState, 2);
                    return returnElement;
                } else {
                    throw exception::LogicException(__FILE__, __LINE__, __func__, std::string("expected table at index ") + std::to_string(index));
                }
            }
            
            template<typename ...T>
            template<unsigned ...S>
            void Exchanger<LuaTuple<T...>>::push(lua_State *luaState, const LuaTuple<T...> &tuple, std::integer_sequence<unsigned, S...>) {
                constexpr unsigned keTupleSize = sizeof...(T);
                if (keTupleSize <= std::numeric_limits<int>::max() || static_cast<int>(keTupleSize) < 0) {
                    lua_createtable(luaState, static_cast<int>(keTupleSize), 0);
                    // stack: table
                    generic::expandDummyTemplatePack((setElementInTable<S + 1, T>(luaState, std::get<S>(tuple), -1), 0)...);
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "LuaTuple is too big");
                }
            }
            
            template<typename ...T>
            template<unsigned ...S>
            void Exchanger<LuaTuple<T...>>::push(lua_State *luaState, T &&...t, std::integer_sequence<unsigned, S...>) {
                constexpr unsigned keTupleSize = sizeof...(T);
                if (keTupleSize <= std::numeric_limits<int>::max() || static_cast<int>(keTupleSize) < 0) {
                    lua_createtable(luaState, static_cast<int>(keTupleSize), 0);
                    // stack: table
                    generic::expandDummyTemplatePack((setElementInTable<S + 1, T>(luaState, std::forward<T>(t), -1), 0)...);
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "LuaTuple is too big");
                }
            }
            
            
            // code replication of: void Exchanger<LuaTuple<T...>>::setElementInTable(lua_State *luaState, const U &element, int index);
            template<typename ...T>
            template<unsigned I, typename U>
            void Exchanger<LuaTuple<T...>>::setElementInTable(lua_State *luaState, U &&element, int index) {
                lua_pushvalue(luaState, index);
                // stack: table
                lua_compatibility::pushunsigned(luaState, I);
                // stack: table | I
                singlePush<U>(luaState, element);
                // stack: table | I | element
                lua_rawset(luaState, -3);
                // stack: table
                lua_pop(luaState, 1);
            }
            
            // code replication of: void Exchanger<LuaTuple<T...>>::setElementInTable(lua_State *luaState, U &&element, int index);
            template<typename ...T>
            template<unsigned I, typename U>
            void Exchanger<LuaTuple<T...>>::setElementInTable(lua_State *luaState, const U &element, int index) {
                lua_pushvalue(luaState, index);
                // stack: table
                lua_compatibility::pushunsigned(luaState, I);
                // stack: table | I
                singlePush<U>(luaState, element);
                // stack: table | I | element
                lua_rawset(luaState, -3);
                // stack: table
                lua_pop(luaState, 1);
            }
            
            template<typename T>
            inline decltype(auto) get(lua_State *luaState, int index) {
                return ExchangerType<T>::get(luaState, index);
            }
            
            template<typename T, typename ...A>
            inline void push(lua_State *luaState, A &&...arguments) {
                static_assert(std::is_reference<T>::value == false, "cannot push reference");
                ExchangerType<T>::push(luaState, std::forward<A>(arguments)...);
            }
            
            template<typename T>
            inline decltype(auto) singleGet(lua_State *luaState, int index) {
                static_assert(IsTemplateClass<LuaPack, generic::BasicType<T>>::value == false, "integral::LuaPack cannot be gotten in this circumstance");
                return get<T>(luaState, index);
            }
            
            template<typename T, typename ...A>
            inline void singlePush(lua_State *luaState, A &&...arguments) {
                static_assert(IsTemplateClass<LuaPack, generic::BasicType<T>>::value == false, "integral::LuaPack cannot be pushed in this circumstance");
                push<T>(luaState, std::forward<A>(arguments)...);
            }
            
            inline void pushCopy(lua_State *luaState) {}
            
            template<typename A, typename ...B>
            void pushCopy(lua_State *luaState, A &&firstArgument, B &&...remainingArguments) {
                push<generic::BasicType<A>>(luaState, std::forward<A>(firstArgument));
                pushCopy(luaState, std::forward<B>(remainingArguments)...);
            }
            
            template<typename ...T>
            inline LuaPack<T...> Exchanger<LuaPack<T...>>::get(lua_State *luaState, int index) {
                return Exchanger<LuaPack<T...>>::get(luaState, index, std::make_integer_sequence<unsigned, sizeof...(T)>());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, const LuaPack<T...> &luaPack) {
                Exchanger<LuaPack<T...>>::push(luaState, luaPack, std::make_integer_sequence<unsigned, sizeof...(T)>());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, T &&...luaPack) {
                Exchanger<LuaPack<T...>>::pushGeneric(luaState, std::forward<T>(luaPack)...);
            }
            
            template<typename ...T>
            template<unsigned ...S>
            LuaPack<T...> Exchanger<LuaPack<T...>>::get(lua_State *luaState, int index, std::integer_sequence<unsigned, S...>) {
                if (index >= 0) {
                    return LuaPack<T...>(ExchangerType<T>::get(luaState, index + S)...);
                } else {
                    return LuaPack<T...>(ExchangerType<T>::get(luaState, index - (sizeof...(S) - 1) + S)...);
                }
            }
            
            template<typename ...T>
            template<unsigned ...S>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, const LuaPack<T...> &luaPack, std::integer_sequence<unsigned, S...>) {
                Exchanger<LuaPack<T...>>::pushGeneric(luaState, std::get<S>(luaPack)...);
            }
            
            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::pushGeneric(lua_State *luaState) {}
            
            template<typename ...T>
            template<typename U, typename ...V>
            void Exchanger<LuaPack<T...>>::pushGeneric(lua_State *luaState, U &&value, V &&...remainingPack) {
                // push in order
                ExchangerType<U>::push(luaState, std::forward<U>(value));
                Exchanger<LuaPack<T...>>::pushGeneric(luaState, std::forward<V>(remainingPack)...);
            }
        }
    }
}

#endif

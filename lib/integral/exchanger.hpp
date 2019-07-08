//
//  exchanger.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016, 2017, 2019  André Pereira Henriques
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

#ifndef integral_exchanger_hpp
#define integral_exchanger_hpp

#include <cstddef>
#include <array>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>
#include <lua.hpp>
#include <exception/Exception.hpp>
#include "ArgumentException.hpp"
#include "basic.hpp"
#include "generic.hpp"
#include "lua_compatibility.hpp"
#include "type_manager.hpp"
#include "UnexpectedStackException.hpp"
#include "UserDataWrapper.hpp"
#include "UserDataWrapperBase.hpp"

namespace integral {
    namespace detail {
        namespace exchanger {
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
                // If the value on the stack is a number, then lua_tostring also changes the actual value in the stack to a string. (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
                static const char * get(lua_State *luaState, int index);

                // for template argument, check http://stackoverflow.com/questions/16708307/is-it-possible-to-legally-overload-a-string-literal-and-const-char
                template<typename T>
                inline static void push(lua_State *luaState, const T * const &string);

                // string literal overload
                template<typename T, std::size_t N>
                inline static void push(lua_State *luaState, const T (&string)[N]);
            };

            template<>
            class Exchanger<std::string> {
            public:
                // If the value on the stack is a number, then lua_tolstring also changes the actual value in the stack to a string. (This change confuses lua_next when lua_tolstring is applied to keys during a table traversal.)
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
            class Exchanger<std::vector<T>> {
            public:
                static std::vector<T> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const std::vector<T> &vector);
            };

            template<typename T, std::size_t N>
            class Exchanger<std::array<T, N>> {
            public:
                static std::array<T, N> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const std::array<T, N> &array);
            };

            template<typename T, typename U>
            class Exchanger<std::unordered_map<T, U>> {
            public:
                static std::unordered_map<T, U> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const std::unordered_map<T, U> &unorderedMap);
            };

            template<typename T>
            class Exchanger<std::optional<T>> {
            public:
                static std::optional<T> get(lua_State *luaState, int index);
                static void push(lua_State *luaState, const std::optional<T>& optional);
            };

            template<typename ...T>
            class Exchanger<std::tuple<T...>> {
            public:
                inline static std::tuple<T...> get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const std::tuple<T...> &tuple);
                inline static void push(lua_State *luaState, T &&...t);

            private:
                template<std::size_t ...S>
                static std::tuple<T...> get(lua_State *luaState, int index, std::index_sequence<S...>);

                template<std::size_t I, typename U>
                static U getElementFromTable(lua_State *luaState, int index);

                template<std::size_t ...S>
                static void push(lua_State *luaState, const std::tuple<T...> &tuple, std::index_sequence<S...>);

                template<std::size_t ...S>
                static void push(lua_State *luaState, T &&...t, std::index_sequence<S...>);

                template<std::size_t I, typename U>
                static void setElementInTable(lua_State *luaState, U &&element, int index);

                template<std::size_t I, typename U>
                static void setElementInTable(lua_State *luaState, const U &element, int index);
            };

            template<typename T>
            using ExchangerType = Exchanger<typename std::decay<T>::type>;

            template<typename T>
            inline decltype(auto) get(lua_State *luaState, int index);

            // throws exception if more than 1 element is pushed onto the stack
            // when "typename T" = LuaFunctionWrapper: nUpValues elements are removed from stack.
            // if more than one element is pushed on the stack, an exception is thrown
            template<typename T, typename ...A>
            void push(lua_State *luaState, A &&...arguments);

            inline void pushCopy(lua_State *luaState);

            template<typename A, typename ...B>
            void pushCopy(lua_State *luaState, A &&firstArgument, B &&...remainingArguments);

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

            template<typename T>
            inline void Exchanger<const char *>::push(lua_State *luaState, const T * const &string) {
                lua_pushstring(luaState, string);
            }

            template<typename T, std::size_t N>
            inline void Exchanger<const char *>::push(lua_State *luaState, const T (&string)[N]) {
                lua_pushlstring(luaState, string, N - 1);
            }

            inline void Exchanger<std::string>::push(lua_State *luaState, const std::string &string) {
                lua_pushlstring(luaState, string.c_str(), string.length());
            }

            template<typename T>
            T Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    const lua_Integer integer = lua_compatibility::tointegerx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(integer);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    const T *userData = type_manager::getConvertibleType<T>(luaState, index);
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
                    const auto unsignedNumber = lua_compatibility::tounsignedx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(unsignedNumber);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    const T *userData = type_manager::getConvertibleType<T>(luaState, index);
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
                    const lua_Number number = lua_compatibility::tonumberx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(number);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    const T *userData = type_manager::getConvertibleType<T>(luaState, index);
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
            std::vector<T> Exchanger<std::vector<T>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, -1);
                        std::vector<T> returnVector;
                        returnVector.reserve(tableSize);
                        for (std::size_t i = 1; i <= tableSize; ++i) {
                            // stack: table
                            lua_compatibility::pushunsigned(luaState, i);
                            // stack: table | i
                            lua_rawget(luaState, -2);
                            // stack: table | luaVectorElement (?)
                            try {
                                returnVector.push_back(exchanger::get<T>(luaState, -1));
                            } catch (const ArgumentException &argumentException) {
                                // stack: table | ?
                                lua_pop(luaState, 2);
                                throw ArgumentException(luaState, index, std::string("invalid table - std::vector - element: " ) + argumentException.what());
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
                    const std::vector<T> *userData = type_manager::getConvertibleType<std::vector<T>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or std::vector");
                    }
                }
            }

            template<typename T>
            void Exchanger<std::vector<T>>::push(lua_State *luaState, const std::vector<T> &vector) {
                using SizeType = typename std::vector<T>::size_type;
                const SizeType vectorSize = vector.size();
                if (vectorSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(vectorSize), 0);
                    // stack: table
                    for (SizeType i = 0; i < vectorSize; ++i) {
                        // stack: table
                        lua_compatibility::pushunsigned(luaState, i + 1);
                        // stack: table | i
                        exchanger::push<T>(luaState, vector.at(i));
                        // stack: table | i | luaVectorElement
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "std::vector is too big");
                }
            }

            template<typename T, std::size_t N>
            std::array<T, N> Exchanger<std::array<T, N>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, -1);
                        if (tableSize == N) {
                            std::array<T, N> returnArray;
                            for (std::size_t i = 1; i <= tableSize; ++i) {
                                // stack: table
                                lua_compatibility::pushunsigned(luaState, i);
                                // stack: table | i
                                lua_rawget(luaState, -2);
                                // stack: table | luaArrayElement (?)
                                try {
                                    returnArray.at(i - 1) = exchanger::get<T>(luaState, -1);
                                } catch (const ArgumentException &argumentException) {
                                    // stack: table | ?
                                    lua_pop(luaState, 2);
                                    throw ArgumentException(luaState, index, std::string("invalid table - std::array - element: " ) + argumentException.what());
                                }
                                // stack: table | luaArrayElement
                                lua_pop(luaState, 1);
                                // stack: table
                            }
                            // stack: table
                            lua_pop(luaState, 1);
                            return returnArray;
                        } else {
                            std::ostringstream errorMessage;
                            errorMessage << "wrong table - std::array - size: expected " << N << ", got " << tableSize;
                            throw ArgumentException(luaState, index, errorMessage.str());
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    const std::array<T, N> *userData = type_manager::getConvertibleType<std::array<T, N>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or std::array");
                    }
                }
            }

            template<typename T, std::size_t N>
            void Exchanger<std::array<T, N>>::push(lua_State *luaState, const std::array<T, N> &array) {
                if (N <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(N), 0);
                    // stack: table
                    for (std::size_t i = 0; i < N; ++i) {
                        // stack: table
                        lua_compatibility::pushunsigned(luaState, i + 1);
                        // stack: table | i
                        exchanger::push<T>(luaState, array.at(i));
                        // stack: table | i | luaArrayElement
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "std::array is too big");
                }
            }

            template<typename T, typename U>
            std::unordered_map<T, U> Exchanger<std::unordered_map<T, U>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        std::unordered_map<T, U> returnUnorderedMap;
                        lua_pushnil(luaState);
                        // atention! the key is pushed again on the stack to preserve its type (so 2 values are popped in each iteration)
                        for (int hasNext = lua_next(luaState, -2); hasNext != 0; lua_pop(luaState, 2), hasNext = lua_next(luaState, -2)) {
                            // stack: table | key (?) | value (?)
                            // stack types can be changed by integral::get (for instance, a number type is converted to string in place in the stack by calling lua_tostring). This corrupts the table traversal. To avoid this, the key value is copied.
                            lua_pushvalue(luaState, -2);
                            // stack: table | key (?) | value (?) | key (?)
                            try {
                                returnUnorderedMap.emplace(exchanger::get<T>(luaState, -1), exchanger::get<U>(luaState, -2));
                            } catch (const ArgumentException &argumentException) {
                                // stack: table | ? | ? | ?
                                lua_pop(luaState, 4);
                                throw ArgumentException(luaState, index, std::string("invalid table - std::unordered_map: " ) + argumentException.what());
                            }
                        }
                        // stack: table
                        lua_pop(luaState, 1);
                        return returnUnorderedMap;

                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    const std::unordered_map<T, U> *userData = type_manager::getConvertibleType<std::unordered_map<T, U>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or std::unordered_map");
                    }
                }
            }

            template<typename T, typename U>
            void Exchanger<std::unordered_map<T, U>>::push(lua_State *luaState, const std::unordered_map<T, U> &unorderedMap) {
                using SizeType = typename std::unordered_map<T, U>::size_type;
                const SizeType unorderedMapSize = unorderedMap.size();
                if (unorderedMapSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(unorderedMapSize), 0);
                    // stack: table
                    for (const auto &keyValue : unorderedMap) {
                        // stack: table
                        exchanger::push<T>(luaState, keyValue.first);
                        // stack: table | key
                        exchanger::push<U>(luaState, keyValue.second);
                        // stack: table | key | value
                        lua_rawset(luaState, -3);
                        // stack: table
                    }
                } else {
                    throw exception::RuntimeException(__FILE__, __LINE__, __func__, "std::unordered_map is too big");
                }
            }

            template<typename T>
            std::optional<T> Exchanger<std::optional<T>>::get(lua_State *luaState, int index) {
                if (lua_isnil(luaState, index) == 0) {
                    return exchanger::get<T>(luaState, index);
                } else {
                    return std::nullopt;
                }
            }

            template<typename T>
            void Exchanger<std::optional<T>>::push(lua_State *luaState, const std::optional<T>& optional) {
                if (optional.has_value() == true) {
                    exchanger::push<T>(luaState, *optional);
                } else {
                    lua_pushnil(luaState);
                }
            }

            template<typename ...T>
            inline std::tuple<T...> Exchanger<std::tuple<T...>>::get(lua_State *luaState, int index) {
                return Exchanger<std::tuple<T...>>::get(luaState, index, std::index_sequence_for<T...>());
            }

            template<typename ...T>
            inline void Exchanger<std::tuple<T...>>::push(lua_State *luaState, const std::tuple<T...> &tuple) {
                Exchanger<std::tuple<T...>>::push(luaState, tuple, std::index_sequence_for<T...>());
            }

            template<typename ...T>
            inline void Exchanger<std::tuple<T...>>::push(lua_State *luaState, T &&...t) {
                Exchanger<std::tuple<T...>>::push(luaState, std::forward<T>(t)..., std::index_sequence_for<T...>());
            }

            template<typename ...T>
            template<std::size_t ...S>
            std::tuple<T...> Exchanger<std::tuple<T...>>::get(lua_State *luaState, int index, std::index_sequence<S...>) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        const std::size_t tableSize = lua_compatibility::rawlen(luaState, index);
                        constexpr std::size_t keTupleSize = sizeof...(T);
                        if (tableSize == keTupleSize) {
                            try {
                                return std::tuple<T...>(getElementFromTable<S + 1, T>(luaState, index)...);
                            } catch (const ArgumentException &argumentException) {
                                throw ArgumentException(luaState, index, std::string("invalid table - std::tuple: " ) + argumentException.what());
                            }
                        } else {
                            std::ostringstream errorMessage;
                            errorMessage << "wrong table - std::tuple - size: expected " <<  keTupleSize << ", got " << tableSize;
                            throw ArgumentException(luaState, index, errorMessage.str());
                        }
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TTABLE));
                    }
                } else {
                    const std::tuple<T...> *userData = type_manager::getConvertibleType<std::tuple<T...>>(luaState, index);
                    if (userData != nullptr) {
                        return *userData;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or std::tuple");
                    }
                }
            }

            template<typename ...T>
            template<std::size_t I, typename U>
            U Exchanger<std::tuple<T...>>::getElementFromTable(lua_State *luaState, int index) {
                if (lua_istable(luaState, index) != 0) {
                    lua_pushvalue(luaState, index);
                    // stack: table
                    lua_compatibility::pushunsigned(luaState, I);
                    // stack: table | i
                    lua_rawget(luaState, -2);
                    // stack: table | luaArrayElement (?)
                    auto returnElement = exchanger::get<U>(luaState, -1);
                    // stack: table | luaArrayElement
                    lua_pop(luaState, 2);
                    return returnElement;
                } else {
                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, "missing table at index ", index);
                }
            }

            template<typename ...T>
            template<std::size_t ...S>
            void Exchanger<std::tuple<T...>>::push(lua_State *luaState, const std::tuple<T...> &tuple, std::index_sequence<S...>) {
                constexpr std::size_t keTupleSize = sizeof...(T);
                if (keTupleSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(keTupleSize), 0);
                    // stack: table
                    generic::expandDummyTemplatePack((setElementInTable<S + 1, T>(luaState, std::get<S>(tuple), -1), 0)...);
                } else {
                    throw exception::LogicException(__FILE__, __LINE__, __func__, "std::tuple is too big");
                }
            }

            template<typename ...T>
            template<std::size_t ...S>
            void Exchanger<std::tuple<T...>>::push(lua_State *luaState, T &&...t, std::index_sequence<S...>) {
                constexpr std::size_t keTupleSize = sizeof...(T);
                if (keTupleSize <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(keTupleSize), 0);
                    // stack: table
                    generic::expandDummyTemplatePack((setElementInTable<S + 1, T>(luaState, std::forward<T>(t), -1), 0)...);
                } else {
                    throw exception::LogicException(__FILE__, __LINE__, __func__, "std::tuple is too big");
                }
            }


            template<typename ...T>
            template<std::size_t I, typename U>
            void Exchanger<std::tuple<T...>>::setElementInTable(lua_State *luaState, U &&element, int index) {
                lua_pushvalue(luaState, index);
                // stack: table
                lua_compatibility::pushunsigned(luaState, I);
                // stack: table | I
                exchanger::push<U>(luaState, std::forward<U>(element));
                // stack: table | I | element
                lua_rawset(luaState, -3);
                // stack: table
                lua_pop(luaState, 1);
            }

            template<typename ...T>
            template<std::size_t I, typename U>
            void Exchanger<std::tuple<T...>>::setElementInTable(lua_State *luaState, const U &element, int index) {
                lua_pushvalue(luaState, index);
                // stack: table
                lua_compatibility::pushunsigned(luaState, I);
                // stack: table | I
                exchanger::push<U>(luaState, element);
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
            void push(lua_State *luaState, A &&...arguments) {
                static_assert(std::is_reference<T>::value == false, "cannot push reference");
                const int stackTopIndex = lua_gettop(luaState);
                ExchangerType<T>::push(luaState, std::forward<A>(arguments)...);
                // stack: ?...
                const int stackIndexDelta = lua_gettop(luaState) - stackTopIndex;
                // when "typename T" = LuaFunctionWrapper: nUpValues elements are removed from stack. That is why the stack is not checked for no element added or for elements removed
                if (stackIndexDelta > 1) {
                    throw UnexpectedStackException(luaState, __FILE__, __LINE__, __func__, stackIndexDelta, " elements pushed onto the stack (expected 1 element) with exchanger::push (type: '", typeid(T).name(), "')");
                }
            }

            inline void pushCopy(lua_State *luaState) {}

            template<typename A, typename ...B>
            void pushCopy(lua_State *luaState, A &&firstArgument, B &&...remainingArguments) {
                push<typename std::decay<A>::type>(luaState, std::forward<A>(firstArgument));
                pushCopy(luaState, std::forward<B>(remainingArguments)...);
            }
        }
    }
}

#endif

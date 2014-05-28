//
//  exchanger.h
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

#ifndef integral_exchanger_h
#define integral_exchanger_h

#include <array>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <lua.hpp>
#include "Adaptor.h"
#include "ArgumentException.h"
#include "basic.h"
#include "TemplateSequence.h"
#include "TemplateSequenceGenerator.h"
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
            
            template<typename ...T>
            using LuaPack = Adaptor<std::tuple<T...>>;
            
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
            private:
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
                static void push(lua_State *luaState, const std::string &string);
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
            
            template<typename ...T>
            class Exchanger<LuaPack<T...>> {
            public:
                inline static LuaPack<T...> get(lua_State *luaState, int index);
                inline static void push(lua_State *luaState, const LuaPack<T...> &luaPack);
                static void push(lua_State *luaState, T &&...luaPack);
                
            private:
                template<unsigned ...S>
                static LuaPack<T...> get(lua_State *luaState, int index, TemplateSequence<S...>);

                template<unsigned ...S>
                inline static void push(lua_State *luaState, const LuaPack<T...> &luaPack, TemplateSequence<S...>);

                inline static void push_(lua_State *luaState);
                
                template<typename U, typename ...V>
                static void push_(lua_State *luaState, U &&value, V &&...remainingPack);
            };

            template<typename T>
            using ExchangerType = Exchanger<basic::BasicType<T>>;
            
            template<typename T>
            inline auto get(lua_State *luaState, int index) -> decltype(ExchangerType<T>::get(luaState, index));
            
            template<typename T, typename ...A>
            inline void push(lua_State *luaState, A &&...arguments);
            
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
            
            template<typename T>
            T Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_signed<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    lua_Integer integer = lua_tointegerx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(integer);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userDataBase = type_manager::getConvertibleType<T>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
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
                    lua_Unsigned unsignedNumber = lua_tounsignedx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(unsignedNumber);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userDataBase = type_manager::getConvertibleType<T>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                }
            }
            
            template<typename T>
            inline void Exchanger<T, typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value>::type>::push(lua_State *luaState, T number) {
                lua_pushunsigned(luaState, static_cast<lua_Unsigned>(number));
            }
            
            inline void Exchanger<bool>::push(lua_State *luaState, bool boolean) {
                lua_pushboolean(luaState, static_cast<int>(boolean));
            }
            
            template<typename T>
            T Exchanger< T, typename std::enable_if<std::is_floating_point<T>::value>::type>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    int isNumber;
                    lua_Number number = lua_tonumberx(luaState, index, &isNumber);
                    if (isNumber != 0) {
                        return static_cast<T>(number);
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
                    }
                } else {
                    T *userDataBase = type_manager::getConvertibleType<T>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
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
                        const std::size_t tableSize = lua_rawlen(luaState, -1);
                        LuaVector<T> returnVector;
                        returnVector.reserve(tableSize);
                        for (std::size_t i = 1; i <= tableSize; ++i) {
                            // stack: table
                            lua_pushunsigned(luaState, i);
                            // stack: table | i
                            lua_rawget(luaState, -2);
                            // stack: table | luaVectorElement (?)
                            try {
                                returnVector.push_back(ExchangerType<T>::get(luaState, -1));
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
                    LuaVector<T> *userDataBase = type_manager::getConvertibleType<LuaVector<T>>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
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
                } else {
                    lua_createtable(luaState, std::numeric_limits<int>::max(), 0);
                }
                // stack: table
                for (SizeType i = 0; i < vectorSize; ++i) {
                    // stack: table
                    lua_pushunsigned(luaState, i + 1);
                    // stack: table | i
                    Exchanger<T>::push(luaState, luaVector.at(i));
                    // stack: table | i | luaVectorElement
                    lua_rawset(luaState, -3);
                    // stack: table
                }
            }
            
            template<typename T, std::size_t N>
            LuaArray<T, N> Exchanger<LuaArray<T, N>>::get(lua_State *luaState, int index) {
                if (lua_isuserdata(luaState, index) == 0) {
                    if (lua_istable(luaState, index) != 0) {
                        lua_pushvalue(luaState, index);
                        // stack: table
                        const std::size_t tableSize = lua_rawlen(luaState, -1);
                        if (tableSize == N) {
                            LuaArray<T, N> returnArray;
                            for (std::size_t i = 1; i <= tableSize; ++i) {
                                // stack: table
                                lua_pushunsigned(luaState, i);
                                // stack: table | i
                                lua_rawget(luaState, -2);
                                // stack: table | luaArrayElement (?)
                                try {
                                    returnArray.at(i - 1) = ExchangerType<T>::get(luaState, -1);
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
                    LuaArray<T, N> *userDataBase = type_manager::getConvertibleType<LuaArray<T, N>>(luaState, index);
                    if (userDataBase != nullptr) {
                        return *userDataBase;
                    } else {
                        throw ArgumentException::createTypeErrorException(luaState, index, "table or LuaArray");
                    }
                }
            }
            
            template<typename T, std::size_t N>
            void Exchanger<LuaArray<T, N>>::push(lua_State *luaState, const LuaArray<T, N> &luaArray) {
                if (N <= std::numeric_limits<int>::max()) {
                    lua_createtable(luaState, static_cast<int>(N), 0);
                } else {
                    lua_createtable(luaState, std::numeric_limits<int>::max(), 0);
                }
                // stack: table
                for (std::size_t i = 0; i < N; ++i) {
                    // stack: table
                    lua_pushunsigned(luaState, i + 1);
                    // stack: table | i
                    Exchanger<T>::push(luaState, luaArray.at(i));
                    // stack: table | i | luaArrayElement
                    lua_rawset(luaState, -3);
                    // stack: table
                }
            }
            
            template<typename ...T>
            inline LuaPack<T...> Exchanger<LuaPack<T...>>::get(lua_State *luaState, int index) {
                return Exchanger<LuaPack<T...>>::get(luaState, index, typename TemplateSequenceGenerator<sizeof...(T)>::TemplateSequenceType());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, const LuaPack<T...> &luaPack) {
                Exchanger<LuaPack<T...>>::push(luaState, luaPack, typename TemplateSequenceGenerator<sizeof...(T)>::TemplateSequenceType());
            }
            
            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, T &&...luaPack) {
                Exchanger<LuaPack<T...>>::push_(luaState, std::forward<T>(luaPack)...);
            }
            
            template<typename ...T>
            template<unsigned ...S>
            LuaPack<T...> Exchanger<LuaPack<T...>>::get(lua_State *luaState, int index, TemplateSequence<S...>) {
                if (index >= 0) {
                    return LuaPack<T...>(Exchanger<T>::get(luaState, index + S)...);
                } else {
                    return LuaPack<T...>(Exchanger<T>::get(luaState, index - (sizeof...(S) - 1) + S)...);
                }
            }
            
            template<typename ...T>
            template<unsigned ...S>
            inline void Exchanger<LuaPack<T...>>::push(lua_State *luaState, const LuaPack<T...> &luaPack, TemplateSequence<S...>) {
                Exchanger<LuaPack<T...>>::push_(luaState, std::get<S>(luaPack)...);
            }

            template<typename ...T>
            inline void Exchanger<LuaPack<T...>>::push_(lua_State *luaState) {}
            
            template<typename ...T>
            template<typename U, typename ...V>
            void Exchanger<LuaPack<T...>>::push_(lua_State *luaState, U &&value, V &&...remainingPack) {
                // push in order
                ExchangerType<U>::push(luaState, std::forward<U>(value));
                Exchanger<LuaPack<T...>>::push_(luaState, std::forward<V>(remainingPack)...);
            }
            
            template<typename T>
            inline auto get(lua_State *luaState, int index) -> decltype(ExchangerType<T>::get(luaState, index)) {
                return ExchangerType<T>::get(luaState, index);
            }
            
            template<typename T, typename ...A>
            inline void push(lua_State *luaState, A &&...arguments) {
                static_assert(std::is_reference<T>::value == false, "cannot push reference");
                ExchangerType<T>::push(luaState, std::forward<A>(arguments)...);
            }
        }
    }
}

#endif

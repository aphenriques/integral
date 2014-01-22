//
//  exchanger.h
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

#ifndef integral_exchanger_h
#define integral_exchanger_h

#include <string>
#include <type_traits>
#include <utility>
#include <lua.hpp>
#include "type_index.h"
#include "type_manager.h"
#include "ArgumentException.h"
#include "basic.h"
#include "UserDataWrapper.h"
#include "UserDataWrapperBase.h"

namespace integral {
    namespace exchanger {
        template<typename T>
        class CompoundExchanger {
        public:
            static T & get(lua_State *luaState, int index);
            
            template<typename ...A>
            static void push(lua_State *luaState, A &&...arguments);
        };
        
        template<typename T>
        class CompoundExchanger<T *> {
        private:
            // Pointers are unsafe
            static T & get(lua_State *luaState, int index) = delete;
            static void push(lua_State *luaState, T *pointer) = delete;
        };
        
        template<>
        class CompoundExchanger<const char *> {
        public:
            static const char * get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, const char *string);
        };
        
        template<>
        class CompoundExchanger<std::string> {
        public:
            inline static std::string get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, const std::string &string);
        };
        
        template<typename T>
        class SignedIntegerExchanger {
        public:
            static T get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, T number);
        };
        
        template<typename T>
        class UnsignedIntegerExchanger {
        public:
            static T get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, T number);
        };
        
        template<>
        class UnsignedIntegerExchanger<bool> {
        public:
            inline static bool get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, bool boolean);
        };
        
        template<typename T>
        class FloatingPointExchanger {
        public:
            inline static T get(lua_State *luaState, int index);
            inline static void push(lua_State *luaState, T number);
        };

        template<typename T>
        using StrippedType = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
        
        template<typename T>
        using ExchangerType = typename std::conditional<std::is_integral<StrippedType<T>>::value,
            typename std::conditional<std::is_signed<StrippedType<T>>::value,
                SignedIntegerExchanger<StrippedType<T>>,
                UnsignedIntegerExchanger<StrippedType<T>>>::type,
            typename std::conditional<std::is_floating_point<StrippedType<T>>::value,
                FloatingPointExchanger<StrippedType<T>>,
                CompoundExchanger<StrippedType<T>>>::type>::type;
        
        
        template<typename T>
        inline auto get(lua_State *luaState, int index) -> decltype(ExchangerType<T>::get(luaState, index));
        
        template<typename T, typename ...A>
        inline void push(lua_State *luaState, A &&...arguments);
        
        //--

        template<typename T>
        T & CompoundExchanger<T>::get(lua_State *luaState, int index) {
            if (lua_isuserdata(luaState, index) != 0) {
                UserDataWrapper<T> *userDataWrapper = type_manager::getUserDataWrapper<T>(luaState, index);
                if (userDataWrapper != nullptr) {
                    return *static_cast<T *>(userDataWrapper);
                } else {
                    UserDataWrapperBase *userDataWrapperBase = type_index::getBaseType<UserDataWrapperBase>(luaState, index);
                    if (userDataWrapperBase != nullptr) {
                        T *castenUserData = dynamic_cast<T *>(userDataWrapperBase);
                        if (castenUserData != nullptr) {
                            return *castenUserData;
                        } else {
                            throw ArgumentException(luaState, index, "incompatible userdata type");
                        }
                    } else {
                        throw ArgumentException(luaState, index, "unknown userdata type or incompatible UserDataWrapperBase objects");
                    }
                }
            } else {
                throw ArgumentException::createTypeErrorException(luaState, index, "userdata");
            }
        }
        
        template<typename T>
        template<typename ...A>
        void CompoundExchanger<T>::push(lua_State *luaState, A &&...arguments) {
            basic::pushUserData<UserDataWrapper<T>>(luaState, std::forward<A>(arguments)...);
            type_manager::pushClassMetatable<T>(luaState); // type_manager will automatically register unknown types
            lua_setmetatable(luaState, -2);
        }
        
        inline void CompoundExchanger<const char *>::push(lua_State *luaState, const char *string) {
            lua_pushstring(luaState, string);
        }
        
        inline std::string CompoundExchanger<std::string>::get(lua_State *luaState, int index) {
            return CompoundExchanger<const char *>::get(luaState, index);
        }
        
        inline void CompoundExchanger<std::string>::push(lua_State *luaState, const std::string &string) {
            lua_pushstring(luaState, string.c_str());
        }
        
        template<typename T>
        T SignedIntegerExchanger<T>::get(lua_State *luaState, int index) {
            int isNumber;
            lua_Integer integer = lua_tointegerx(luaState, index, &isNumber);
            if (isNumber == 0) {
                throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
            }
            return static_cast<T>(integer);
        }
        
        template<typename T>
        inline void SignedIntegerExchanger<T>::push(lua_State *luaState, T number) {
            lua_pushinteger(luaState, static_cast<lua_Integer>(number));
        }
        
        template<typename T>
        T UnsignedIntegerExchanger<T>::get(lua_State *luaState, int index) {
            int isNumber;
            lua_Unsigned unsignedNumber = lua_tounsignedx(luaState, index, &isNumber);
            if (isNumber == 0) {
                throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
            }
            return static_cast<T>(unsignedNumber);
        }
        
        template<typename T>
        inline void UnsignedIntegerExchanger<T>::push(lua_State *luaState, T number) {
            lua_pushunsigned(luaState, static_cast<lua_Unsigned>(number));
        }
        
        inline bool UnsignedIntegerExchanger<bool>::get(lua_State *luaState, int index) {
            return lua_toboolean(luaState, index);
        }
        
        inline void UnsignedIntegerExchanger<bool>::push(lua_State *luaState, bool boolean) {
            lua_pushboolean(luaState, static_cast<int>(boolean));
        }
        
        template<typename T>
        T FloatingPointExchanger<T>::get(lua_State *luaState, int index) {
            int isNumber;
            lua_Number number = lua_tonumberx(luaState, index, &isNumber);
            if (isNumber == 0) {
                throw ArgumentException::createTypeErrorException(luaState, index, lua_typename(luaState, LUA_TNUMBER));
            }
            return static_cast<T>(number);
        }
        
        template<typename T>
        inline void FloatingPointExchanger<T>::push(lua_State *luaState, T number) {
            lua_pushnumber(luaState, static_cast<lua_Number>(number));
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

#endif

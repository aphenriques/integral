//
//  Caller.h
//  integral
//
//  Copyright (C) 2014, 2016  André Pereira Henriques
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

#ifndef integral_Caller_h
#define integral_Caller_h

#include <string>
#include <type_traits>
#include <lua.hpp>
#include "CallerException.h"
#include "exchanger.h"
#include "generic.h"
#include "type_count.h"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class Caller {
        public:
            static decltype(auto) call(lua_State *luaState, const A &...arguments);
        };
        
        template<typename ...A>
        class Caller<void, A...> {
        public:
            static void call(lua_State *luaState, const A &...arguments);
        };
        
        //--
        
        template<typename R, typename ...A>
        decltype(auto) Caller<R, A...>::call(lua_State *luaState, const A &...arguments) {
            static_assert(generic::getLogicalOr(std::is_reference<generic::StringLiteralFilterType<A>>::value...) == false, "Caller arguments can not be pushed as reference. They are pushed by value");
            exchanger::pushCopy(luaState, arguments...);
            if (lua_pcall(luaState, type_count::getTypeCount<A...>(), type_count::getTypeCount<R>(), 0) == lua_compatibility::keLuaOk) {
                decltype(auto) returnValue = exchanger::get<R>(luaState, -1);
                lua_pop(luaState, type_count::getTypeCount<R>());
                return returnValue;
            } else {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }
        
        template<typename ...A>
        void Caller<void, A...>::call(lua_State *luaState, const A &...arguments) {
            static_assert(generic::getLogicalOr(std::is_reference<generic::StringLiteralFilterType<A>>::value...) == false, "Caller arguments can not be pushed as reference. They are pushed by value");
            exchanger::pushCopy(luaState, arguments...);
            if (lua_pcall(luaState, type_count::getTypeCount<A...>(), 0, 0) != lua_compatibility::keLuaOk) {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }
    }
}

#endif

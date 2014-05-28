//
//  Caller.h
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

#ifndef integral_Caller_h
#define integral_Caller_h

#include <string>
#include <utility>
#include <lua.hpp>
#include "basic.h"
#include "CallerException.h"
#include "exchanger.h"
#include "type_counter.h"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class Caller {
        public:
            static auto call(lua_State *luaState, A &&...arguments) -> decltype(exchanger::get<R>(luaState, -1));
        };
        
        template<typename ...A>
        class Caller<void, A...> {
        public:
            static void call(lua_State *luaState, A &&...arguments);
        };
        
        //--
        
        template<typename R, typename ...A>
        auto Caller<R, A...>::call(lua_State *luaState, A &&...arguments) -> decltype(exchanger::get<R>(luaState, -1)) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            if (lua_pcall(luaState, type_counter::getCount<A...>(), type_counter::getCount<R>(), 0) == LUA_OK) {
                using ReturnType = decltype(exchanger::get<R>(luaState, -1));
                ReturnType returnValue = exchanger::get<R>(luaState, -1);
                lua_pop(luaState, type_counter::getCount<R>());
                return returnValue;
            } else {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }
        
        template<typename ...A>
        void Caller<void, A...>::call(lua_State *luaState, A &&...arguments) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            lua_call(luaState, type_counter::getCount<A...>(), 0);
        }
    }
}

#endif

//
//  Caller.hpp
//  integral
//
//  Copyright (C) 2014, 2016, 2017  André Pereira Henriques
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

#ifndef integral_Caller_hpp
#define integral_Caller_hpp

#include <stdexcept>
#include <string>
#include <type_traits>
#include <lua.hpp>
#include "exception/TemplateClassException.hpp"
#include "ArgumentException.hpp"
#include "exchanger.hpp"
#include "generic.hpp"
#include "IsStringLiteral.hpp"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class Caller {
        public:
            static decltype(auto) call(lua_State *luaState, A &&...arguments);
        };

        template<typename ...A>
        class Caller<void, A...> {
        public:
            static void call(lua_State *luaState, A &&...arguments);
        };

        using CallerException = exception::TemplateClassException<Caller, std::runtime_error>;

        //--

        template<typename R, typename ...A>
        decltype(auto) Caller<R, A...>::call(lua_State *luaState, A &&...arguments) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            if (lua_pcall(luaState, sizeof...(A), 1, 0) == lua_compatibility::keLuaOk) {
                try {
                    decltype(auto) returnValue = exchanger::get<R>(luaState, -1);
                    lua_pop(luaState, 1);
                    return returnValue;
                } catch (const ArgumentException &argumentException) {
                    // the number of results from lua_pcall is adjusted to 'nresults' argument
                    lua_pop(luaState, 1);
                    throw ArgumentException(luaState, -1, std::string("invalid return type: " ) + argumentException.what());
                }
            } else {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }

        template<typename ...A>
        void Caller<void, A...>::call(lua_State *luaState, A &&...arguments) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            if (lua_pcall(luaState, sizeof...(A), 0, 0) != lua_compatibility::keLuaOk) {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }
    }
}

#endif

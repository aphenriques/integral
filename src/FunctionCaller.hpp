//
//  FunctionCaller.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016  André Pereira Henriques
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

#ifndef integral_FunctionCaller_hpp
#define integral_FunctionCaller_hpp

#include <cstddef>
#include <functional>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class FunctionCaller {
        public:        
            template<std::size_t ...S>
            static int call(lua_State *luaState, const std::function<R(A...)> &function, std::integer_sequence<std::size_t, S...>);
        };

        template<typename ...A>
        class FunctionCaller<void, A...> {
        public:        
            template<std::size_t ...S>
            static int call(lua_State *luaState, const std::function<void(A...)> &function, std::integer_sequence<std::size_t, S...>);
        };

        //--

        template<typename R, typename ...A>
        template<std::size_t ...S>
        int FunctionCaller<R, A...>::call(lua_State *luaState, const std::function<R(A...)> &function, std::integer_sequence<std::size_t, S...>) {
            exchanger::push<R>(luaState, function(exchanger::get<A>(luaState, S + 1)...));
            return 1;
        }

        template<typename ...A>
        template<std::size_t ...S>
        int FunctionCaller<void, A...>::call(lua_State *luaState, const std::function<void(A...)> &function, std::integer_sequence<std::size_t, S...>) {
            function(exchanger::get<A>(luaState, S + 1)...);
            return 0;
        }
    }
}

#endif

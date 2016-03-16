//
//  FunctionCaller.h
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

#ifndef integral_FunctionCaller_h
#define integral_FunctionCaller_h

#include <functional>
#include <utility>
#include <lua.hpp>
#include "exchanger.h"
#include "type_count.h"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class FunctionCaller {
        public:        
            template<unsigned ...S>
            static unsigned call(lua_State *luaState, const std::function<R(A...)> &function, std::integer_sequence<unsigned, S...>);
        };
        
        template<typename ...A>
        class FunctionCaller<void, A...> {
        public:        
            template<unsigned ...S>
            static unsigned call(lua_State *luaState, const std::function<void(A...)> &function, std::integer_sequence<unsigned, S...>);
        };
        
        //--

        template<typename R, typename ...A>
        template<unsigned ...S>
        unsigned FunctionCaller<R, A...>::call(lua_State *luaState, const std::function<R(A...)> &function, std::integer_sequence<unsigned, S...>) {
            exchanger::push<R>(luaState, function(exchanger::get<A>(luaState, S + 1)...));
            return static_cast<int>(type_count::getTypeCount<R>());
        }
        
        template<typename ...A>
        template<unsigned ...S>
        unsigned FunctionCaller<void, A...>::call(lua_State *luaState, const std::function<void(A...)> &function, std::integer_sequence<unsigned, S...>) {
            function(exchanger::get<A>(luaState, S + 1)...);
            return 0;
        }
    }
}

#endif

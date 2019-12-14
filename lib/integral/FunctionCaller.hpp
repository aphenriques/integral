//
//  FunctionCaller.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016, 2017, 2019  André Pereira Henriques
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
            static int call(lua_State *luaState, const std::function<R(A...)> &function, std::index_sequence<S...>);
        };

        template<typename ...A>
        class FunctionCaller<void, A...> {
        public:
            template<std::size_t ...S>
            static int call(lua_State *luaState, const std::function<void(A...)> &function, std::index_sequence<S...>);
        };

        //--

        template<typename R, typename ...A>
        template<std::size_t ...S>
        int FunctionCaller<R, A...>::call(lua_State *luaState, const std::function<R(A...)> &function, std::index_sequence<S...>) {
            exchanger::push<R>(luaState, function(exchanger::get<A>(luaState, S + 1)...));
            return 1;
        }

        template<typename ...A>
        template<std::size_t ...S>
        // [[maybe_unused]] is used because of a bug in gcc 7.4 which incorrectly shows the following warning:
        // error: parameter ‘luaState’ set but not used [-Werror=unused-but-set-parameter]
        // FIXME remove [[maybe_unused]] in future versions
        int FunctionCaller<void, A...>::call([[maybe_unused]] lua_State *luaState, const std::function<void(A...)> &function, std::index_sequence<S...>) {
            function(exchanger::get<A>(luaState, S + 1)...);
            return 0;
        }
    }
}

#endif

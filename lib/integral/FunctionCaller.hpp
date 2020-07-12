//
//  FunctionCaller.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

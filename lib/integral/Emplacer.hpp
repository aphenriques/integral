//
//  Emplacer.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_Emplacer_hpp
#define integral_Emplacer_hpp

#include <tuple>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"

namespace integral {
    namespace detail {
        template<typename T, typename ...A>
        class Emplacer {
        public:
            inline Emplacer(A &&...arguments);

            inline void push(lua_State *luaState) const &;
            inline void push(lua_State *luaState) &&;

        private:
            std::tuple<A...> tuple_;

            template<std::size_t ...S>
            inline void push(lua_State *luaState, std::index_sequence<S...>) const &;

            template<std::size_t ...S>
            inline void push(lua_State *luaState, std::index_sequence<S...>) &&;
        };

        namespace exchanger {
            template<typename T, typename ...A>
            class Exchanger<Emplacer<T, A...>> {
            public:
                template<typename U>
                inline static void push(lua_State *luaState, U &&emplacer);
            };
        }

        //--

        template<typename T, typename ...A>
        inline Emplacer<T, A...>::Emplacer(A &&...arguments) : tuple_(std::forward<A>(arguments)...) {}

        template<typename T, typename ...A>
        inline void Emplacer<T, A...>::push(lua_State *luaState) const & {
            push(luaState, std::index_sequence_for<A...>());
        }

        template<typename T, typename ...A>
        inline void Emplacer<T, A...>::push(lua_State *luaState) && {
            std::move(*this).push(luaState, std::index_sequence_for<A...>());
        }

        template<typename T, typename ...A>
        template<std::size_t ...S>
        inline void Emplacer<T, A...>::push(lua_State *luaState, std::index_sequence<S...>) const & {
            exchanger::push<T>(luaState, std::get<S>(tuple_)...);
        }

        template<typename T, typename ...A>
        template<std::size_t ...S>
        inline void Emplacer<T, A...>::push(lua_State *luaState, std::index_sequence<S...>) && {
            exchanger::push<T>(luaState, std::move(std::get<S>(tuple_))...);
        }

        namespace exchanger {
            template<typename T, typename ...A>
            template<typename U>
            inline void Exchanger<Emplacer<T, A...>>::push(lua_State *luaState, U &&emplacer) {
                std::forward<U>(emplacer).push(luaState);
            }
        }
    }
}

#endif

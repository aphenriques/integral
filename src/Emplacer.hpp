//
//  Emplacer.hpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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

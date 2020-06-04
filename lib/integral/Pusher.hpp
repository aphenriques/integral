//
//  Pusher.hpp
//  integral
//
//  Copyright (C) 2020  André Pereira Henriques
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

#ifndef integral_Pusher_hpp
#define integral_Pusher_hpp

#include <functional>
#include <utility>
#include <lua.hpp>
#include "exchanger.hpp"

namespace integral {
    class Pusher {
    public:
        // non-copyable
        Pusher(const Pusher &) = delete;
        Pusher & operator=(const Pusher &) = delete;

        Pusher(Pusher &&) = default;

        template<typename T>
        inline Pusher(T &&pushFunction);

        inline void push(lua_State *luaState) const;

    private:
        std::function<void(lua_State *)> pushFunction_;
    };

    namespace detail {
        namespace exchanger {
            template<>
            class Exchanger<Pusher> {
            public:
                inline static void push(lua_State *luaState, const Pusher &pusher);

                template<typename T>
                inline static void push(lua_State *luaState, const T &pushFunction);
            };
        }
    }

    //--

    template<typename T>
    inline Pusher::Pusher(T &&pushFunction) : pushFunction_(std::forward<T>(pushFunction)) {}

    inline void Pusher::push(lua_State *luaState) const {
        pushFunction_(luaState);
    }

    namespace detail {
        namespace exchanger {
            inline void Exchanger<Pusher>::push(lua_State *luaState, const Pusher &pusher) {
                pusher.push(luaState);
            }

            template<typename T>
            inline void Exchanger<Pusher>::push(lua_State *luaState, const T &pushFunction) {
                pushFunction(luaState);
            }
        }
    }
}

#endif

//
//  Pusher.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

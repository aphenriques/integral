//
//  Global.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_Global_hpp
#define integral_Global_hpp

#include "GlobalBase.hpp"
#include "ReferenceValue.hpp"

namespace integral {
    class Global : public GlobalBase {
    public:
        using GlobalBase::push;

        template<typename K>
        detail::ReferenceValue<std::decay_t<K>, Global> operator[](K &&key) &&;
    };

    namespace detail::exchanger {
        template<>
        class Exchanger<Global> {
        public:
            inline static void push(lua_State *luaState, const Global &global);
            inline static void push(lua_State *luaState);
        };
    }

    //--

    template<typename K>
    detail::ReferenceValue<std::decay_t<K>, Global> Global::operator[](K &&key) && {
        return {std::forward<K>(key), std::move(*this)};
    }

    namespace detail::exchanger {
        inline void Exchanger<Global>::push(lua_State *luaState, const Global &global) {
            global.push(luaState);
        }

        inline void Exchanger<Global>::push(lua_State *luaState) {
            Global().push(luaState);
        }
    }
}

#endif


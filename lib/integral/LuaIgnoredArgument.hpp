//
//  LuaIgnoredArgument.hpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#ifndef integral_LuaIgnoredArgument_hpp
#define integral_LuaIgnoredArgument_hpp

#include <lua.hpp>
#include "exchanger.hpp"

namespace integral {
    namespace detail {
        class LuaIgnoredArgument {
        public:
            // non-copyable
            LuaIgnoredArgument(const LuaIgnoredArgument &) = delete;
            LuaIgnoredArgument & operator=(const LuaIgnoredArgument &) = delete;

            LuaIgnoredArgument(LuaIgnoredArgument &&) = default;
            LuaIgnoredArgument() = default;
        };

        namespace exchanger {
            template<>
            class Exchanger<LuaIgnoredArgument> {
            public:
                inline static LuaIgnoredArgument get(lua_State *luaState, int index);
                //Caution! A possible (not recommended) push method that pushes nothing onto the lua stack would require adaptations in the whole codebase because exchanger::push is (mostly) expected to push a single element on the stack
            };
        }

        //--

        namespace exchanger {
            inline LuaIgnoredArgument Exchanger<LuaIgnoredArgument>::get(lua_State * /*luaState*/, int /*index*/) {
                return LuaIgnoredArgument();
            }
        }
    }
}

#endif

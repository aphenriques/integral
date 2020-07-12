//
//  LuaFunctionArgument.hpp
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

#ifndef integral_LuaFunctionArgument_hpp
#define integral_LuaFunctionArgument_hpp

#include <lua.hpp>
#include "Caller.hpp"
#include "exchanger.hpp"

namespace integral {
    namespace detail {
        class LuaFunctionArgument {
        public:
            // non-copyable
            LuaFunctionArgument(const LuaFunctionArgument &) = delete;
            LuaFunctionArgument & operator=(const LuaFunctionArgument &) = delete;

            LuaFunctionArgument(LuaFunctionArgument &&) = default;

            LuaFunctionArgument(lua_State *luaState, int index);

            template<typename R, typename ...A>
            inline decltype(auto) call(A &&...arguments) const;

        private:
            lua_State * const luaState_;
            const int luaAbsoluteStackIndex_;
        };

        namespace exchanger {
            template<>
            class Exchanger<LuaFunctionArgument> {
            public:
                inline static LuaFunctionArgument get(lua_State *luaState, int index);
            };
        }

        // --

        template<typename R, typename ...A>
        inline decltype(auto) LuaFunctionArgument::call(A &&...arguments) const {
            lua_pushvalue(luaState_, luaAbsoluteStackIndex_);
            return Caller<R, A...>::call(luaState_, std::forward<A>(arguments)...);
        }

        namespace exchanger {
            inline LuaFunctionArgument Exchanger<LuaFunctionArgument>::get(lua_State *luaState, int index) {
                // element at index stack position is not checked with lua_isfunction because it might not be a function and have a valid __call metamethod
                return LuaFunctionArgument(luaState, index);
            }
        }
    }
}

#endif

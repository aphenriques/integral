//
//  Caller.hpp
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

#ifndef integral_Caller_hpp
#define integral_Caller_hpp

#include <stdexcept>
#include <string>
#include <type_traits>
#include <lua.hpp>
#include <exception/TemplateClassException.hpp>
#include "ArgumentException.hpp"
#include "exchanger.hpp"
#include "generic.hpp"
#include "IsStringLiteral.hpp"

namespace integral {
    namespace detail {
        template<typename R, typename ...A>
        class Caller {
        public:
            static decltype(auto) call(lua_State *luaState, A &&...arguments);
        };

        template<typename ...A>
        class Caller<void, A...> {
        public:
            static void call(lua_State *luaState, A &&...arguments);
        };

        using CallerException = exception::TemplateClassException<Caller, std::runtime_error>;

        //--

        template<typename R, typename ...A>
        decltype(auto) Caller<R, A...>::call(lua_State *luaState, A &&...arguments) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            if (lua_pcall(luaState, sizeof...(A), 1, 0) == lua_compatibility::keLuaOk) {
                try {
                    decltype(auto) returnValue = exchanger::get<R>(luaState, -1);
                    lua_pop(luaState, 1);
                    return returnValue;
                } catch (const ArgumentException &argumentException) {
                    // the number of results from lua_pcall is adjusted to 'nresults' argument
                    lua_pop(luaState, 1);
                    throw ArgumentException(luaState, -1, std::string("invalid return type: " ) + argumentException.what());
                }
            } else {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }

        template<typename ...A>
        void Caller<void, A...>::call(lua_State *luaState, A &&...arguments) {
            exchanger::pushCopy(luaState, std::forward<A>(arguments)...);
            if (lua_pcall(luaState, sizeof...(A), 0, 0) != lua_compatibility::keLuaOk) {
                std::string errorMessage(lua_tostring(luaState, -1));
                lua_pop(luaState, 1);
                throw CallerException(errorMessage);
            }
        }
    }
}

#endif

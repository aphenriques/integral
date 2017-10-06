//
//  LuaFunctionArgument.hpp
//  integral
//
//  Copyright (C) 2014, 2016, 2017  André Pereira Henriques
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

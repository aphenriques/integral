//
//  LuaIgnoredArgument.hpp
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

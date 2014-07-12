//
//  LuaIgnoredArgument.h
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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

#ifndef integral_LuaIgnoredArgument_h
#define integral_LuaIgnoredArgument_h

#include <lua.hpp>
#include "exchanger.h"

namespace integral {
    namespace detail {
        class LuaIgnoredArgument {
            template<typename, typename> friend class exchanger::Exchanger;
            
        public:
            LuaIgnoredArgument(LuaIgnoredArgument &&) = default;
            
        protected:
            LuaIgnoredArgument() = default;
            
            // LuaIgnoredArgument cannot not be copied
            LuaIgnoredArgument(const LuaIgnoredArgument &) = delete;
            LuaIgnoredArgument & operator=(const LuaIgnoredArgument &) = delete;
        };
        
        namespace exchanger {
            template<>
            class Exchanger<LuaIgnoredArgument> {
            public:
                inline static LuaIgnoredArgument get(lua_State *luaState, int index);
            };
        }
        
        //--
        
        namespace exchanger {
            inline LuaIgnoredArgument Exchanger<LuaIgnoredArgument>::get(lua_State *luaState, int index) {
                return LuaIgnoredArgument();
            }
        }
    }
}

#endif

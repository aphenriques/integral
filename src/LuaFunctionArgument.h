//
//  LuaFunctionArgument.h
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

#ifndef integral_LuaFunctionArgument_h
#define integral_LuaFunctionArgument_h

#include <lua.hpp>
#include "Caller.h"
#include "exchanger.h"

namespace integral {
    namespace detail {
        class LuaFunctionArgument {
            template<typename, typename> friend class exchanger::Exchanger;
            
        private:
            lua_State * const luaState_;
            const int luaAbsoluteStackIndex_;
            
            LuaFunctionArgument(lua_State *luaState, int index);
            // LuaFunctionArgument cannot not be copied
            LuaFunctionArgument(const LuaFunctionArgument &) = delete;
            LuaFunctionArgument & operator=(const LuaFunctionArgument &) = delete;
            
        public:
            LuaFunctionArgument(LuaFunctionArgument &&) = default;
            
            // definition in class body because GCC reports strange errors from inside decltype when class definition is done separately. There is no such problem in Clang.
            template<typename R, typename ...A>
            auto call(const A &...arguments) const -> decltype(Caller<R, A...>::call(luaState_, arguments...)) {
                lua_pushvalue(luaState_, luaAbsoluteStackIndex_);
                return Caller<R, A...>::call(luaState_, arguments...);
            }
        };
        
        namespace exchanger {
            template<>
            class Exchanger<LuaFunctionArgument> {
            public:
                static LuaFunctionArgument get(lua_State *luaState, int index);
            };
        }
    }
}

#endif
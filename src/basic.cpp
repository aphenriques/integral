//
//  basic.cpp
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

#include "basic.h"

namespace integral {
    namespace detail {
        namespace basic {
            const char * const gkUnknownExceptionMessage = "[integral] unknown exception thrown";
            
            void setLuaFunction(lua_State *luaState, const char *name, lua_CFunction function, int nUpValues) {
                lua_pushcclosure(luaState, function, nUpValues);
                lua_pushstring(luaState, name);
                lua_insert(luaState, -2);
                lua_rawset(luaState, -3);
            }
        }
    }
}

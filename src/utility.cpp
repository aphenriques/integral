//
//  utility.cpp
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

#include "utility.h"
#include <iostream>

namespace integral {
    namespace utility {
        int printStack(lua_State *luaState) {
            int top = lua_gettop(luaState);
            std::cout << "stack dump: ";
            for (int i = 1; i <= top; i++) {
                int t = lua_type(luaState, i);
                switch (t) {
                    case LUA_TSTRING:
                        std::cout << '"' << lua_tostring(luaState, i) << '"';
                        break;
                        
                    case LUA_TBOOLEAN:
                        std::cout <<(lua_toboolean(luaState, i) ? "true" : "false");
                        break;
                        
                    case LUA_TNUMBER:
                        std::cout << lua_tonumber(luaState, i);
                        break;
                        
                    case LUA_TLIGHTUSERDATA:
                        std::cout << "lightuserdata";
                        break;
                        
                    default:
                        if (lua_iscfunction(luaState, i) == true) {
                            std::cout << "cfunction";
                        } else {
                            std::cout << lua_typename(luaState, t);
                        }
                        break;
                        
                }
                std::cout << "  ";
            }
            std::cout << std::endl;
            return 0;
        }
    }
}

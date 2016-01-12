//
//  utility.h
//  integral
//
//  Copyright (C) 2013, 2014, 2016  André Pereira Henriques
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

#ifndef integral_utility_h
#define integral_utility_h

#include <functional>
#include <initializer_list>
#include <tuple>
#include <lua.hpp>

namespace integral {
    namespace utility {
        int printStack(lua_State *luaState);
        void setHelp(lua_State *luaState, const char *field, const char *fieldDescription);
        void setWithHelp(lua_State *luaState, const char *field, const char *fieldDescription, const std::function<void(lua_State *)> &pushFunction);
        void pushNameAndValueList(lua_State *luaState, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
        void setNameAndValueListWithHelp(lua_State *luaState, const char *field, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
    }
}

#endif

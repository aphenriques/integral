//
//  utility.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2016, 2019  André Pereira Henriques
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

#ifndef integral_utility_hpp
#define integral_utility_hpp

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <string>
#include <tuple>
#include <utility>
#include <lua.hpp>

namespace integral {
    namespace utility {
        std::string getStackString(lua_State *luaState);

        int printStack(lua_State *luaState);

        // stack argument: table
        void setHelp(lua_State *luaState, const char *field, const char *fieldDescription);

        // stack argument: table | ?
        void setWithHelp(lua_State *luaState, const char *field, const char *fieldDescription);

        void pushNameAndValueList(lua_State *luaState, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);

        // stack argument: table
        void setNameAndValueListWithHelp(lua_State *luaState, const char *field, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
    }
}

#endif

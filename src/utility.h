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
#include <utility>
#include <lua.hpp>
#include "core.h"

namespace integral {
    namespace utility {
        int printStack(lua_State *luaState);
        
        // stack argument: table
        void setHelp(lua_State *luaState, const char *field, const char *fieldDescription);
        
        // stack argument: table | ?
        void setWithHelp(lua_State *luaState, const char *field, const char *fieldDescription);
        
        // stack argument: table
        template<typename T, typename ...E, unsigned ...I>
        void setLuaFunctionWithHelp(lua_State *luaState, const char *functionName, const char *functionDescription, T &&luafunction, DefaultArgument<E, I> &&...defaultArguments);
        
        // stack argument: table
        template<typename T, typename ...E, unsigned ...I>
        void setFunctionWithHelp(lua_State *luaState, const char *functionName, const char *functionDescription, T &&function, DefaultArgument<E, I> &&...defaultArguments);
        
        void pushNameAndValueList(lua_State *luaState, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
        
        // stack argument: table
        void setNameAndValueListWithHelp(lua_State *luaState, const char *field, std::initializer_list<std::tuple<const char *, int>> nameAndValueList);
        
        //--
        
        template<typename T, typename ...E, unsigned ...I>
        void setFunctionWithHelp(lua_State *luaState, const char *functionName, const char *functionDescription, T &&function, DefaultArgument<E, I> &&...defaultArguments) {
            integral::setFunction(luaState, functionName, std::forward<T>(function), std::move(defaultArguments)...);
            setHelp(luaState, functionName, functionDescription);
        }
        
        template<typename T, typename ...E, unsigned ...I>
        void setLuaFunctionWithHelp(lua_State *luaState, const char *functionName, const char *functionDescription, T &&function, DefaultArgument<E, I> &&...defaultArguments) {
            integral::setLuaFunction(luaState, functionName, std::forward<T>(function), std::move(defaultArguments)...);
            setHelp(luaState, functionName, functionDescription);
        }
    }
}

#endif

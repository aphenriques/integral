//
//  LuaFunctionWrapper.h
//  integral
//
//  Copyright (C) 2013  André Pereira Henriques
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

#ifndef integral_LuaFunctionWrapper_h
#define integral_LuaFunctionWrapper_h

#include <string>
#include <functional>
#include <lua.hpp>
#include "basic.h"

namespace integral {
    // This class exists to make it possible to get exceptions thrown by luaFunction_ on FunctionWrapperBase::callFunctionWrapper. For example, this is important to get exceptions thrown by exchanger::callConstructor when there are wrong parameters (exchanger::Exchange throws). 
    class LuaFunctionWrapper {
    public:
        static void setFunction(lua_State *luaState, const std::string &name, const LuaFunctionWrapper &luaFunction, int nUpValues = 0);
        
        inline LuaFunctionWrapper(const std::function<int(lua_State *)> &luaFunction);
        inline LuaFunctionWrapper(lua_CFunction luaFunction);
        
        inline int call(lua_State *luaState) const;
        
    private:
        static const char * const kMetatableName_;

        std::function<int(lua_State *)> luaFunction_;
    };
    
    //--
    
    inline LuaFunctionWrapper::LuaFunctionWrapper(const std::function<int(lua_State *)> &luaFunction) : luaFunction_(luaFunction) {}

    inline LuaFunctionWrapper::LuaFunctionWrapper(lua_CFunction luaFunction) : LuaFunctionWrapper(std::function<int(lua_State *)>(luaFunction)) {}

    inline int LuaFunctionWrapper::call(lua_State *luaState) const {
        return luaFunction_(luaState);
    }
}

#endif

//
//  ArgumentException.h
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

#ifndef integral_ArgumentException_h
#define integral_ArgumentException_h

#include <exception>
#include <string>
#include <lua.hpp>

namespace integral {
    class ArgumentException : public std::exception {
    public:
        static ArgumentException createTypeErrorException(lua_State *luaState, int index, const std::string &userDataName);
        
        ArgumentException(lua_State *luaState, int index, const std::string &extraMessage);
        ArgumentException(lua_State *luaState, unsigned maximumNumberOfArguments, unsigned actualNumberOfArguments);
        
        inline const char * what() const noexcept override;
        
    private:
        static bool findField (lua_State *luaState, int index, int level);        
        static bool pushGlobalFunctionName (lua_State *L, lua_Debug *debugInfo);
        
        std::string message_;
    };
    
    //--
    
    inline const char * ArgumentException::what() const noexcept {
        return message_.c_str();
    }
}


#endif

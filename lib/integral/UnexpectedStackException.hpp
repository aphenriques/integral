//
//  UnexpectedStackException.hpp
//  integral
//
//  Copyright (C) 2016, 2019  André Pereira Henriques
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

#ifndef integral_UnexpectedStackException_hpp
#define integral_UnexpectedStackException_hpp

#include <lua.hpp>
#include <exception/Exception.hpp>
#include "utility.hpp"

namespace integral {
    class UnexpectedStackException : public exception::LogicException {
    public:
        template<typename ...A>
        UnexpectedStackException(lua_State *luaState, const char *fileName, int lineNumber, const char *functionName, const A &...messageParts);

        ~UnexpectedStackException();
    };

    //--

    template<typename ...A>
    UnexpectedStackException::UnexpectedStackException(
        lua_State *luaState,
        const char *fileName,
        int lineNumber,
        const char *functionName,
        const A &...messageParts
    ) :
        ::exception::LogicException(
            fileName,
            lineNumber,
            functionName,
            "[integral] invalid Lua stack: ",
            messageParts...,
            ". lua stack: { ",
            utility::getStackString(luaState),
            " }"
        )
    {}
}

#endif /* integral_UnexpectedStackException_hpp */

//
//  GlobalBase.hpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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

#ifndef integral_GlobalBase_hpp
#define integral_GlobalBase_hpp

#include <string>
#include <lua.hpp>
#include "lua_compatibility.hpp"

namespace integral {
    class GlobalBase {
    public:
        // non-copyable
        GlobalBase(const GlobalBase &) = delete;
        GlobalBase & operator=(const GlobalBase &) = delete;

        GlobalBase(GlobalBase &&) = default;
        GlobalBase() = default;

        inline std::string getReferenceString() const;

    protected:
        inline void push(lua_State *luaState) const;
    };

    //--

    inline std::string GlobalBase::getReferenceString() const {
        return "_G";
    }

    inline void GlobalBase::push(lua_State *luaState) const {
        detail::lua_compatibility::pushglobaltable(luaState);
    }
}

#endif


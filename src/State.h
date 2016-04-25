//
//  State.h
//  integral
//
//  Copyright (C) 2016  André Pereira Henriques
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

#ifndef integral_State_h
#define integral_State_h

#include <memory>
#include <string>
#include <lua.hpp>

namespace integral {
    class State {
    public:
        // throws StateException on failure
        // defines State::atPanic as lua panic function
        State();
        
        inline lua_State * getLuaState() const;
        inline void openLibs() const;
        
        // throws StateCallException on error
        void doString(const std::string &string) const;
    
        // throws StateCallException on error
        void doFile(const std::string &fileName) const;
        
    private:
        std::shared_ptr<lua_State> luaState_;
        
        // throws StateException
        [[noreturn]] static int atPanic(lua_State *luaState);
    };
    
    //--
    
    inline lua_State * State::getLuaState() const {
        return luaState_.get();
    }

    inline void State::openLibs() const {
        luaL_openlibs(getLuaState());
    }
}

#endif

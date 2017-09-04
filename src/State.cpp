//
//  State.cpp
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

#include "State.hpp"
#include <string>
#include <lua.hpp>
#include "exception/Exception.hpp"

namespace integral {
    State::State() try : StateView(luaL_newstate()) {
    } catch (const StateException &stateException) {
        throw exception::RuntimeException(__FILE__, __LINE__, __func__, std::string("[integral] failed to create new lua state: { ") + stateException.what() + " }");
    }

    State::~State() {
        if (getLuaState() != nullptr) {
            lua_close(getLuaState());
        }
    }
}
//
//  Container.cpp
//  integral
//
//  Copyright (C) 2014, 2015  André Pereira Henriques
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

#include <exception>
#include <functional>
#include <lua.hpp>
#include "integral.hpp"

class Id {
public:
    double number_;
};

class Container {
public:
    Id id_;

    Container(double number) : id_{number} {}
};

extern "C" {
    LUALIB_API int luaopen_libContainer(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Container>(luaState);
            integral::setConstructor<Container, double>(luaState, "new");

            // type function from Container to Id (similar to C++ type operator)
            integral::defineTypeFunction(luaState, [](Container *container) -> Id * {
                return &container->id_;
            });

            // this function takes an Id object as parameter
            integral::setFunction(luaState, "getNumberFromId", [](const Id &id) {
                return id.number_;
            });

            // type function from Container to double
            integral::defineTypeFunction(luaState, [](Container *container) {
                return &container->id_.number_;
            });

            // this function takes a double as parameter
            integral::setFunction(luaState, "getNegativeNumber", [](double number) {
                return -number;
            });

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[type_function sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[type_function sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


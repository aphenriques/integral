//
//  VectorOfDoubles.cpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016  André Pereira Henriques
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

#include <functional>
#include <string>
#include <lua.hpp>
#include "integral.hpp"

extern "C" {
    LUALIB_API int luaopen_libVectorOfDoubles(lua_State *luaState) {
        try {
            using VectorOfDoubles = std::vector<double>;
            integral::pushClassMetatable<VectorOfDoubles>(luaState);
            integral::setConstructor<VectorOfDoubles()>(luaState, "new");
            // because of some legacy lua implementation details, __len receives two arguments, the second argument can be safely ignored
            integral::setFunction(luaState, "__len", [](const VectorOfDoubles &vector, integral::LuaIgnoredArgument) -> std::size_t {
                return vector.size();
            });
            // explicit cast is necessary to avoid ambiguity because std::vector<T>::push_back is an overloaded function
            integral::setFunction(luaState, "pushBack", static_cast<void(VectorOfDoubles::*)(const double &)>(&VectorOfDoubles::push_back));
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[ignored_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[ignored_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

//
//  Regex.cpp
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

#include <string>
#include <regex>
#include <functional>
#include <utility>
#include <lua.hpp>
#include "integral.h"
#include "Match.h"

extern "C" {
    LUALIB_API int luaopen_libRegex(lua_State *luaState) {
        try {
            using Regex = std::regex;

            // Match class
            integral::core::pushClassMetatable<Match>(luaState);
            // Methods
            integral::core::setFunction(luaState, "__tostring", std::function<std::string(const Match &)>(std::bind(&Match::str, std::placeholders::_1, 0)));
            integral::core::setFunction(luaState, "getSize", &Match::size);
            integral::core::setFunction(luaState, "__call", &Match::getSubMatch);
            // Everything registered
            lua_pop(luaState, 1);
            // Match class could be pushed back again to register other functions with all its functions already registered (nothing is lost)

            // SubMatch class
            integral::core::pushClassMetatable<SubMatch>(luaState);
            // Methods
            integral::core::setFunction(luaState, "__tostring", &SubMatch::str);
            // Everything registered
            lua_pop(luaState, 1);
            // SubMatch class could be pushed back again to register other functions with all its functions already registered (nothing is lost)

            // Attention! There is NO requirement for the order in which the classes and its functons are registered

            // Regex class
            integral::core::pushClassMetatable<Regex>(luaState);
            // Constructor
            integral::core::setConstructor<Regex, std::string>(luaState, "new");

            const std::pair<const char *, Match::Mode> namesAndModes[] = {{"match" , Match::Mode::EXACT  },
                                                                          {"search", Match::Mode::PARTIAL}};
            for (auto &nameAndMode : namesAndModes) {
                Match::Mode mode = nameAndMode.second;
                // Lua functions can be registered. Just like luaL_setfuncs (lua_CFunction). But they can also be functors and can handle exceptions graciously
                integral::core::setLuaFunction(luaState, nameAndMode.first, [mode](lua_State *luaState) -> int {
                    // Get fisrt lua function argument
                    Regex pattern = integral::core::get<Regex>(luaState, 1);
                    // Get second lua function argument
                    const char * string = integral::core::get<const char *>(luaState, 2);
                    Match match(string, pattern, mode);
                    if (match.empty() == false) {
                        // Push return value
                        integral::core::push<Match>(luaState, match);
                    } else {
                        // Push return value
                        lua_pushnil(luaState);
                    }
                    return 1;
                });
                // Regex metatable is not popped because it is what is returned by the module
            }
            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[Regex sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[Regex sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

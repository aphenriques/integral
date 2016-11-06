//
//  Regex.cpp
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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
#include <regex>
#include <string>
#include <utility>
#include <lua.hpp>
#include "integral.hpp"
#include "Match.hpp"

extern "C" {
    LUALIB_API int luaopen_libRegex(lua_State *luaState) {
        try {
            using Regex = std::regex;

            // Match class
            integral::pushClassMetatable<Match>(luaState);
            // Methods
            integral::setFunction(luaState, "__tostring", std::function<std::string(const Match &)>(std::bind(&Match::str, std::placeholders::_1, 0)));
            integral::setFunction(luaState, "getSize", &Match::size);
            integral::setFunction(luaState, "__call", &Match::getSubMatch);
            lua_pop(luaState, 1);
            // Match class could be pushed back again to register other functions with all its functions already registered (nothing is lost)

            // SubMatch class
            integral::pushClassMetatable<SubMatch>(luaState);
            // Methods
            integral::setFunction(luaState, "__tostring", &SubMatch::str);
            lua_pop(luaState, 1);
            // SubMatch class could be pushed back again to register other functions with all its functions already registered (nothing is lost)

            // Regex class
            integral::pushClassMetatable<Regex>(luaState);
            // Constructor
            integral::setConstructor<Regex(std::string)>(luaState, "new");

            const std::pair<const char *, Match::Mode> namesAndModes[] = {{"match" , Match::Mode::EXACT  },
                                                                          {"search", Match::Mode::PARTIAL}};
            for (auto &nameAndMode : namesAndModes) {
                Match::Mode mode = nameAndMode.second;
                // Lua functions can be registered. Just like luaL_setfuncs (lua_CFunction). But they can also be functors and can handle exceptions graciously
                integral::setLuaFunction(luaState, nameAndMode.first, [mode](lua_State *lambdaLuaState) -> int {
                    // Get fisrt lua function argument
                    Regex pattern = integral::get<Regex>(lambdaLuaState, 1);
                    // Get second lua function argument
                    const char * string = integral::get<const char *>(lambdaLuaState, 2);
                    Match match(string, pattern, mode);
                    if (match.empty() == false) {
                        // Push return value
                        integral::push<Match>(lambdaLuaState, match);
                    } else {
                        // Push return value
                        lua_pushnil(lambdaLuaState);
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

//
//  Regex.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <functional>
#include <regex>
#include <string>
#include <utility>
#include <lua.hpp>
#include <integral/integral.hpp>
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

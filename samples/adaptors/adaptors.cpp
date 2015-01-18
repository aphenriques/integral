//
//  adaptors.cpp
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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

#include <array>
#include <iostream>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <lua.hpp>
#include "integral.h"

extern "C" {
    LUALIB_API int luaopen_libadaptors(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            // LuaVector
            integral::setFunction(luaState, "getVectorSample", std::function<integral::LuaVector<int>()>([]() -> integral::LuaVector<int> {
                return std::vector<int>({1, 2, 3});
            }));
            // nested LuaVectors work as expected
            integral::setFunction(luaState, "printVectorOfVectors", std::function<void(integral::LuaVector<integral::LuaVector<int>>)>([](integral::LuaVector<integral::LuaVector<int>> luaVectorOfVectors) -> void {
                std::cout << "vector of vectors (C++): {";
                for (const auto &element : luaVectorOfVectors) {
                    std::cout << " {";
                    for (const auto &j : element) {
                        std::cout << " " << j;
                    }
                    std::cout << " }";
                }
                std::cout << " }" << std::endl;
            }));

            // LuaArray
            // adaptors with integral::get/integral::push
            // nested adaptors will also work as expected
            integral::setLuaFunction(luaState, "getArrayOf3VectorsSample", [](lua_State *luaState) -> int {
                integral::push<integral::LuaArray<integral::LuaVector<int>, 3>>(luaState, std::array<integral::LuaVector<int>, 3>{{std::vector<int>({11}), std::vector<int>({21, 22}), std::vector<int>({31, 32, 33})}});
                return 1;
            });

            integral::setLuaFunction(luaState, "printArrayOf2Numbers", [](lua_State *luaState) -> int {
                std::array<int, 2> array = integral::get<integral::LuaArray<int, 2>>(luaState, 1);
                std::cout << "array (C++): {";
                for (const auto &element : array ) {
                    std::cout << " " << element;
                }
                std::cout << " }" << std::endl;
                return 0;
            });

            // LuaUnorderedMap
            integral::setFunction(luaState, "getUnorderedMapSample", std::function<integral::LuaUnorderedMap<std::string, int>()>([]() -> integral::LuaUnorderedMap<std::string, int> {
                return std::unordered_map<std::string, int>({{"um", 1}, {"dois", 2}, {"tres", 3}});
            }));

            integral::setFunction(luaState, "printStringDoubleUnorderedMap", std::function<void(integral::LuaUnorderedMap<std::string, double>)>([](integral::LuaUnorderedMap<std::string, double> map) -> void {
                std::cout << "unordered_map<string, double> (C++): {";
                for (const auto &keyValue : map) {
                    std::cout << " [\"" << keyValue.first << "\"]=" << keyValue.second;
                }
                std::cout << " }" << std::endl;
            }));

            // LuaTuple
            integral::setFunction(luaState, "getTupleSample", std::function<integral::LuaTuple<std::string, double>()>([]() -> integral::LuaTuple<std::string, double> {
                return integral::LuaTuple<std::string, double>("first", 2.0);
            }));

            integral::setFunction(luaState, "printBoolBoolTuple", std::function<void(integral::LuaTuple<bool, bool>)>([](integral::LuaTuple<bool, bool> tuple) -> void {
                std::cout << "tuple<bool, bool> (C++): ( " << std::get<0>(tuple) << ", " << std::get<1>(tuple) << " )" << std::endl;
            }));

            // LuaPack
            integral::setFunction(luaState, "getPackSample", std::function<integral::LuaPack<int, bool, std::string>()>([]() -> integral::LuaPack<int, bool, std::string> {
                return integral::LuaPack<int, bool, std::string>(42, false, "test_string");
            }));

            integral::setFunction(luaState, "printBoolBoolPack", std::function<void(integral::LuaPack<bool, bool>)>([](integral::LuaPack<bool, bool> pack) -> void {
                std::cout << "pack<bool, bool> (C++): " << std::get<0>(pack) << ", " << std::get<1>(pack) << std::endl;
            }));

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[adaptors sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[adaptors sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

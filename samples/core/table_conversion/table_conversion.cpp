//
//  table_conversion.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <algorithm>
#include <array>
#include <iostream>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <lua.hpp>
#include <integral/integral.hpp>

std::vector<int> getVectorSample() {
    return {1, 2, 3};
}

std::vector<int> getDoubleVector(std::vector<int> vectorCopy) {
    std::for_each(vectorCopy.begin(), vectorCopy.end(), [](int &x) -> void {
        x *= 2;
    });
    return vectorCopy;
}

void printVectorOfVectors(const std::vector<std::vector<int>> &vectorOfVectors) {
    std::cout << "vector of vectors (C++): {";
    for (const auto &element : vectorOfVectors) {
        std::cout << " {";
        for (const auto &j : element) {
            std::cout << " " << j;
        }
        std::cout << " }";
    }
    std::cout << " }" << std::endl;
}

extern "C" {
    LUALIB_API int luaopen_libtable_conversion(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            // std::vector
            integral::setFunction(luaState, "getVectorSample", getVectorSample);
            integral::setFunction(luaState, "getDoubleVector", getDoubleVector);
            // nested std::vector work as expected
            integral::setFunction(luaState, "printVectorOfVectors", printVectorOfVectors);

            // std::array
            // nested table types will also work as expected
            integral::setLuaFunction(luaState, "getArrayOf3VectorsSample", [](lua_State *lambdaLuaState) -> int {
                integral::push<std::array<std::vector<int>, 3>>(lambdaLuaState, std::array<std::vector<int>, 3>{std::vector<int>({11}), std::vector<int>({21, 22}), std::vector<int>({31, 32, 33})});
                return 1;
            });

            integral::setLuaFunction(luaState, "printArrayOf2Numbers", [](lua_State *lambdaLuaState) -> int {
                std::array<int, 2> array = integral::get<std::array<int, 2>>(lambdaLuaState, 1);
                std::cout << "array (C++): {";
                for (const auto &element : array ) {
                    std::cout << " " << element;
                }
                std::cout << " }" << std::endl;
                return 0;
            });

            // std::unordered_map
            integral::setFunction(luaState, "getUnorderedMapSample", []() -> std::unordered_map<std::string, int> {
                return {{"um", 1}, {"dois", 2}, {"tres", 3}};
            });

            integral::setFunction(luaState, "printStringDoubleUnorderedMap", [](std::unordered_map<std::string, double> map) -> void {
                std::cout << "unordered_map<string, double> (C++): {";
                for (const auto &keyValue : map) {
                    std::cout << " [\"" << keyValue.first << "\"]=" << keyValue.second;
                }
                std::cout << " }" << std::endl;
            });

            // std::tuple
            integral::setFunction(luaState, "getTupleSample", []() -> std::tuple<std::string, double> {
                return std::tuple<std::string, double>("first", 2.0);
            });

            integral::setFunction(luaState, "printBoolBoolTuple", [](std::tuple<bool, bool> tuple) -> void {
                std::cout << "tuple<bool, bool> (C++): ( " << std::get<0>(tuple) << ", " << std::get<1>(tuple) << " )" << std::endl;
            });

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[table_conversion sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[table_conversion sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

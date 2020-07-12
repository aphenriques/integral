//
//  table_conversion.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <cstdlib>
#include <numeric>
#include <array>
#include <functional>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    Object(const std::string &name) : name_(name) {}
    const std::string & getName() const {
        return name_;
    }

private:
    const std::string name_;
};

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        // std::vector
        luaState["intVector"] = std::vector<int>{1, 2, 3};
        luaState.doString("print(intVector[1] .. ' ' .. intVector[2] .. ' ' ..  intVector[3])"); // prints "1 2 3"

        // std::array
        luaState.doString("arrayOfVectors = {{'one', 'two'}, {'three'}}");
        std::array<std::vector<std::string>, 2> arrayOfVectors = luaState["arrayOfVectors"];
        std::cout << arrayOfVectors.at(0).at(0) << ' ' << arrayOfVectors.at(0).at(1) << ' ' << arrayOfVectors.at(1).at(0) << '\n';

        // std::unordered_map and std::tuple
        luaState["mapOfTuples"] = std::unordered_map<std::string, std::tuple<int, double>>{{"one", {-1, -1.1}}, {"two", {1, 4.2}}};
        luaState.doString("print(mapOfTuples.one[1] .. ' ' .. mapOfTuples.one[2] .. ' ' .. mapOfTuples.two[1] .. ' ' .. mapOfTuples.two[2])");

        luaState["Object"] = integral::ClassMetatable<Object>()
                             .setConstructor<Object(const std::string &)>("new")
                             .setFunction("getNameCopy", std::function<std::string(Object &)>(&Object::getName));
        luaState.doString("mapOfObjects = {x = Object.new('o1'), y = Object.new('o2')}");
        std::unordered_map<std::string, Object> mapOfObjects = luaState["mapOfObjects"];
        std::cout << "x: " << mapOfObjects.at("x").getName()
                  << "\ny: " << mapOfObjects.at("y").getName() << '\n';

        luaState["accumulateIntegers"].setFunction([](const std::vector<int> &vector, int initialValue, const integral::LuaFunctionArgument &binaryOperation) {
            return std::accumulate(vector.cbegin(), vector.cend(), initialValue, [&binaryOperation](int x, int y) {
                return binaryOperation.call<int>(x, y);
            });
        });
        luaState.doString("print('sum 1 to 5: ' .. accumulateIntegers({1, 2, 3, 4, 5}, 0, function(x, y) return x + y end))");
        luaState.doString("print('product 1 to 5: ' .. accumulateIntegers({1, 2, 3, 4, 5}, 1, function(x, y) return x*y end))");

        luaState["getUnorderedMapSample"].setFunction([]() -> std::unordered_map<std::string, Object> {
            return {{"x", {"o1"}}, {"y", {"o2"}}};
        });
        luaState.doString("unorderedMapSample = getUnorderedMapSample()\n"
                          "print('{x = ' .. unorderedMapSample.x:getNameCopy() .. ', y = ' .. unorderedMapSample.y:getNameCopy() .. '}')");

        try {
            luaState.doString("v = {1, 'two', 3}");
            luaState["v"].get<std::vector<int>>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            luaState.doString("v = {x = 'a', [1] = 'b'}");
            luaState["v"].get<std::unordered_map<int, std::string>>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[table_conversion] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

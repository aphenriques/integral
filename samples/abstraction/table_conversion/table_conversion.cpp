//
//  table_conversion.cpp
//  integral
//
//  Copyright (C) 2017  André Pereira Henriques
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

#include <cstdlib>
#include <numeric>
#include <array>
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <lua.hpp>
#include <integral.hpp>

class Object {
public:
    Object(const std::string &name) : name_(name) {}
    const std::string & getName() const {
        return name_;
    }

private:
    const std::string name_;
};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();
        luaState["vectorOfVectors"] = std::vector<std::vector<int>>{{1, 2}, {3}};
        luaState.doString("print(vectorOfVectors[1][1] .. ' ' .. vectorOfVectors[1][2] .. ' ' ..  vectorOfVectors[2][1])");
        luaState.doString("arrayOfVectors = {{'one', 'two'}, {'three'}}");
        std::array<std::vector<std::string>, 2> arrayOfVectors = luaState["arrayOfVectors"];
        std::cout << arrayOfVectors.at(0).at(0) << ' ' << arrayOfVectors.at(0).at(1) << ' ' << arrayOfVectors.at(1).at(0) << '\n';
        luaState["mapOfTuples"] = std::unordered_map<std::string, std::tuple<int, double>>{{"one", {-1, -1.1}}, {"two", {1, 4.2}}};
        luaState.doString("print(mapOfTuples.one[1] .. ' ' .. mapOfTuples.one[2] .. ' ' .. mapOfTuples.two[1] .. ' ' .. mapOfTuples.two[2])");
        luaState["Object"] = integral::ClassMetatable<Object>()
                             .setConstructor<Object(const std::string &)>("new");
        luaState.doString("mapOfObjects = {x = Object.new('o1'), y = Object.new('o2')}");
        std::unordered_map<std::string, Object> mapOfObjects = luaState["mapOfObjects"];
        std::cout << "x: " << mapOfObjects.at("x").getName()
                  << "\ny: " << mapOfObjects.at("y").getName() << '\n';
        luaState["accumulateIntegers"] = integral::makeFunctionWrapper([](const std::vector<int> &vector, int initialValue, const integral::LuaFunctionArgument &binaryOperation) {
            return std::accumulate(vector.cbegin(), vector.cend(), initialValue, [&binaryOperation](int x, int y) {
                return binaryOperation.call<int>(x, y);
            });
        });
        luaState.doString("print('sum 1 to 5: ' .. accumulateIntegers({1, 2, 3, 4, 5}, 0, function(x, y) return x + y end))");
        luaState.doString("print('product 1 to 5: ' .. accumulateIntegers({1, 2, 3, 4, 5}, 1, function(x, y) return x*y end))");
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

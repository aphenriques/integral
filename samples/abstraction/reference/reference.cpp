//
//  reference.cpp
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
#include <iostream>
#include <lua.hpp>
#include <integral.hpp>

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState.doString("a = 42");
        std::cout << "cpp: a = " << luaState["a"].get<int>() << '\n';

        luaState["b"].set("forty two");
        luaState.doString("print('lua: b = ' .. b)\n"
                          "c = .42");
        double c = luaState["c"];
        std::cout << "cpp: c = " << c << '\n';

        luaState["d"] = "quarenta e dois";
        luaState.doString("print('lua: d = ' .. d)\n"
                          "t = {'x', {pi = 3.14}}");
        std::cout << "cpp: t[1] = " << luaState["t"][1].get<const char *>() << '\n';
        std::cout << "cpp: t[2].pi = " << luaState["t"][2]["pi"].get<double>() << '\n';
        luaState["t"]["key"] = "value";
        luaState.doString("print('lua: t.key = ' .. t.key)");

        try {
            luaState["x"].get<std::string>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            luaState["t"]["x"]["x"] = true;
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            // FIXME in c++ 17: [[maybe_unused]] int x = luaState["x"];
            int x = luaState["x"];
            // FIXME with c++17 remove the following line, which silences unused variable warning
            (void)x;
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[reference] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

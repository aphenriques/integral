//
//  function_call.cpp
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
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

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState.doString("function getSum(x, y) return x + y end");
        int x = luaState["getSum"].call<int>(2, 3);
        std::cout << "x = " << x << '\n';

        luaState["printMessage"].setFunction([](const char *message) {
            std::puts(message);
        }, integral::DefaultArgument<const char *, 1>("default message!"));
        luaState["printMessage"].call<void>("hello!");
        luaState["printMessage"].call<void>();

        luaState["Object"] = integral::ClassMetatable<Object>()
                             .setConstructor<Object(const std::string &)>("new");
        luaState.doString("setmetatable(Object, {__call = function(self, ...) return self.new(...) end})");
        std::cout << luaState["Object"]["new"].call<Object>("Object.new(...)").getName() << '\n';
        std::cout << luaState["Object"].call<Object>("Object(...)").getName() << '\n';
        std::vector<int> vector = luaState["table"]["pack"].call<std::vector<int>>(1, 2, 3, 4, 5);
        std::cout << "vector = [ ";
        for(const auto &i : vector) {
            std::cout << i << ' ';
        }
        std::cout << "]\n";

        try {
            luaState["undefined"].call<void>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            luaState["getSum"].call<int>(2, "string");
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        try {
            luaState["getSum"].call<std::vector<int>>(2, 3);
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[function_call] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

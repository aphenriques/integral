//
//  optional.cpp
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

        luaState.doString("function f(optional) if optional == nil then print('lua: hi!') else print('lua: hi ' .. optional .. '!')  end end");
        std::optional<std::string> message(std::nullopt);
        luaState["f"].call<void>(message); // prints "lua: hi!"
        message.emplace("world");
        luaState["f"].call<void>(message); // prints "lua: hi world!"
        luaState["g"].setFunction([](const std::optional<std::string> &optional) {
            if (optional.has_value() == false) {
                std::cout << "c++: hi!" << std::endl;
            } else {
                std::cout << "c++: hi " << optional.value() << '!' << std::endl;
            }
        });
        luaState.doString("g()"); // prints "c++: hi!"
        luaState.doString("g('world')"); // prints "c++: hi world!"
        std::optional<std::string> match;
        match = luaState["string"]["match"].call<std::optional<std::string>>("aei123bcd", "123");
        std::cout << match.value() << std::endl; // prints "123"
        match = luaState["string"]["match"].call<std::optional<std::string>>("aei123bcd", "xyz");
        std::cout << match.has_value() << std::endl; // prints "0"

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[optional] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

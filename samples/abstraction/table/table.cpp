//
//  table.cpp
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

#include <cmath>
#include <iostream>
#include <lua.hpp>
#include <integral.hpp>

class Object {
public:
    Object(const std::string &name) : name_(name) {}

    std::string getHello() const {
        return std::string("Hello ") + name_ + '!';
    }

private:
    std::string name_;
};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();
        luaState["group"] = integral::Table()
                                .set("constant", integral::Table()
                                    .set("pi", 3.14))
                                .setFunction("printHello", []{
                                    std::puts("Hello!");
                                })
                                .set("Object", integral::ClassMetatable<Object>()
                                    .setConstructor<Object(const std::string &)>("new")
                                    .setFunction("getHello", &Object::getHello));
        luaState.doString("print(group.constant.pi)\n"
                          "group.printHello()\n"
                          "print(group.Object.new('object'):getHello())");
        try {
            luaState["group"]["pi"].get<double>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }
        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[table] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

//
//  class.cpp
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

#include <iostream>
#include <lua.hpp>
#include <integral.hpp>

class Object {
public:
    std::string name_;

    Object(const std::string &name) : name_(name) {}

    std::string getHello() const {
        return std::string("Hello ") + name_ + '!';
    }
};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();
        luaState["Object"] = integral::ClassMetatable<Object>()
                             .setConstructor<Object(const std::string &)>("new")
                             .setCopyGetter("getName", &Object::name_)
                             .setSetter("setName", &Object::name_)
                             .setFunction("getHello", &Object::getHello)
                             .setFunction("getBye", [](const Object &object) {
                                return std::string("Bye ") + object.name_ + '!';
                             })
                             .setLuaFunction("appendName", [](lua_State *lambdaLuaState) {
                                // objects are gotten by reference
                                integral::get<Object>(lambdaLuaState, 1).name_ += integral::get<const char *>(lambdaLuaState, 2);
                                return 1;
                             });
        luaState.doString("object = Object.new('foo')\n"
                          "print(object:getName())\n"
                          "object:setName('bar')\n"
                          "print(object:getHello())");
        // objects are gotten by reference
        luaState["object"].get<Object>().name_ = "foobar";
        luaState.doString("print(object:getName())\n"
                          "object:appendName('foo')\n"
                          "print(object:getBye())");

        try {
            luaState.doString("object.getName()");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        try {
            luaState.doString("object:setName('x', 42)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        try {
            luaState.doString("object:appendName(object)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[class] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

//
//  default_argument.cpp
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
    Object(const std::string &name) : name_(name) {}

    void printMessage(const std::string &message) const {
        std::cout << name_ << ": " << message << '\n';
    }
private:
    std::string name_;
};

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        // invalid integral::DefaultArgument index and/or type generates compile time errors, both setting function or constructor
        luaState["Object"] = integral::ClassMetatable<Object>()
                                 .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 1>("default name"))
                                 // compile error: invalid default argument index
                                 // .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 0>("default name"))
                                 // compile error: invalid default argument type
                                 // .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<int, 1>(42))
                                 // compile error: more then 1 default argument definitions for a single argument
                                 // .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 1>("default name"), integral::DefaultArgument<std::string, 1>("other name"))
                                 .setFunction("printMessage", &Object::printMessage, integral::DefaultArgument<Object, 1>("default object"), integral::DefaultArgument<std::string, 2>("default message"));
        luaState.doString("object1 = Object.new(\"defined name\")\n"
                          "object1:printMessage(\"defined message\")\n"
                          "object1:printMessage()\n"
                          "object2 = Object.new()\n"
                          "object2:printMessage(\"defined message\")\n"
                          "object2:printMessage()\n"
                          "Object.printMessage(nil, \"defined message\")\n"
                          "Object.printMessage()\n");
        luaState["printArguments"].setFunction([](const std::string &string, int integer) {
            std::cout << string << ", " << integer << '\n';
        }, integral::DefaultArgument<std::string, 1>("default string"), integral::DefaultArgument<int, 2>(-1));
        luaState.doString("printArguments(\"defined string\")");
        luaState.doString("printArguments(nil, 42)");
        luaState.doString("printArguments()");
        luaState["printStringAndNumber"].setFunction([](const std::string &string, double number) {
            std::cout << string << ", " << number << '\n';
        }, integral::DefaultArgument<std::string, 1>("undefined"));
        luaState.doString("printStringAndNumber(nil, 0.1)");
        try {
            luaState.doString("printStringAndNumber(nil, nil)");
        } catch (const integral::StateException &stateException) {
            std::cout << "expected exception: {" << stateException.what() << "}\n";
        }
        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[default_argument] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

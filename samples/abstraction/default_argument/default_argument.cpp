//
//  default_argument.cpp
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

#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    Object(const std::string &name) : name_(name) {}

    void printMessage(const std::string &message) const {
        std::cout << name_ << ": " << message << '\n';
    }
private:
    std::string name_;
};

int main() {
    try {
        integral::State luaState;

        // invalid integral::DefaultArgument index and/or type generates compile time errors, both setting function or constructor
        luaState["Object"] = integral::ClassMetatable<Object>()
                                 .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 1>("default name"))
                                 // compile error: invalid default argument index
                                 // .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<std::string, 0>("default name"))
                                 // compile error: invalid default argument type
                                 // .setConstructor<Object(const std::string &)>("new", integral::DefaultArgument<int, 1>(42))
                                 // compile error: more then 1 default argument definition for a single argument
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
        luaState.doString("printArguments(\"defined string\")\n"
                          "printArguments(nil, 42)\n"
                          "printArguments()");

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

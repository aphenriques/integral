//
//  class.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    const std::string greeting_;
    std::string name_;

    Object(const std::string greeting, const std::string &name) : greeting_(greeting), name_(name) {}

    // const reference values are pushed by value (copied) to the Lua state
    const std::string & getGreeting() const {
        return greeting_;
    }

    std::string getHello() const {
        return greeting_ + ' ' + name_ + '!';
    }
};

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["Object"] = integral::ClassMetatable<Object>()
                                 // invalid constructor register causes compilation error
                                 // .setConstructor<Object()>("invalid")
                                 .setConstructor<Object(const std::string &, const std::string &)>("new")
                                 .setFunction("getGreeting", &Object::getGreeting)
                                 .setGetter("getName", &Object::name_)
                                 .setSetter("setName", &Object::name_)
                                 .setFunction("getHello", &Object::getHello)
                                 .setFunction("getBye", [](const Object &object) {
                                    return std::string("Bye ") + object.name_ + '!';
                                 })
                                 .setLuaFunction("appendName", [](lua_State *lambdaLuaState) {
                                    // objects (except std::vector, std::array, std::unordered_map, std::tuple and std::string) are gotten by reference
                                    integral::get<Object>(lambdaLuaState, 1).name_ += integral::get<const char *>(lambdaLuaState, 2);
                                    return 1;
                                 });
        luaState.doString("object = Object.new('hi', 'foo')\n"
                          "print(object:getGreeting())\n"
                          "print(object:getName())\n"
                          "object:setName('bar')\n"
                          "print(object:getHello())");

        // objects (except std::vector, std::array, std::unordered_map, std::tuple and std::string) are gotten by reference
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

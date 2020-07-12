//
//  function_call.cpp
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

int main() {
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

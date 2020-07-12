//
//  optional.cpp
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

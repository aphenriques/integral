//
//  embedded.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2013, 2014, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
    void printMessage() const {
        std::cout << "Object " << this << " message!" << std::endl;
    }
};

int main() {
    lua_State *luaState = nullptr;

    try {
        luaState = luaL_newstate();
        if (luaState == nullptr) {
            return 1;
        }

        integral::pushClassMetatable<Object>(luaState);
        integral::setConstructor<Object()>(luaState, "new");
        // lua function name need not be the same as the C++ function name
        integral::setFunction(luaState, "print", &Object::printMessage);
        lua_setglobal(luaState, "Object");

        luaL_dostring(luaState, "local object = Object.new()\n"
                                "object:print()");

        lua_close(luaState);
        return 0;
    } catch (const std::exception &exception) {
        std::cout << "[embedded sample] " << exception.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception thrown" << std::endl;
    }

    lua_close(luaState);
    return 1;
}

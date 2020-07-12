//
//  reference_wrapper.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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

#include <functional>
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
        luaL_openlibs(luaState);

        integral::pushClassMetatable<Object>(luaState);
        integral::setFunction(luaState, "print", &Object::printMessage);

        lua_pop(luaState, 1);

        std::cout << "C++:" << std::endl;
        Object object;
        object.printMessage();

        // std::reference_wrapper<T> has automatic synthetic inheritance do T as if it was defined as:
        // integral::defineInheritance(luaState, [](std::reference_wrapper<T> *referenceWrapperPointer) -> T * {
        //     return &referenceWrapperPointer->get();
        // });
        integral::push<std::reference_wrapper<Object>>(luaState, object);
        lua_setglobal(luaState, "objectReference");

        luaL_dostring(luaState, "print('lua:')\n"
                                "objectReference:print()");

        lua_close(luaState);
        return 0;
    } catch (const std::exception &exception) {
        std::cout << "[reference_wrapper sample] " << exception.what() << std::endl;
    } catch (...) {
        std::cout << "unknown exception thrown" << std::endl;
    }

    lua_close(luaState);
    return 1;
}

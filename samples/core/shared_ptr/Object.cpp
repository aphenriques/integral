//
//  Object.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2014, 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <functional>
#include <memory>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    void print() const {
        std::cout << "Object::print" << std::endl;
    }
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);
            integral::setFunction(luaState, "print", &Object::print);
            integral::setFunction(luaState, "getShared", &std::make_shared<Object>);

            // std::shared_ptr<T> has automatic synthetic inheritance do T as if it was defined as:
            // integral::defineInheritance(luaState, [](std::shared_ptr<T> *sharedPtrPointer) -> T * {
            //     return sharedPtrPointer->get();
            // });
            // or:
            // integral::defineInheritance(luaState, std::function<T *(std::shared_ptr<T> *)>(&std::shared_ptr<T>::get));
            // the following statement may not work if the get method is from a base class of std::shared_ptr (may fail in some stdlib implementations)
            // integral::defineInheritance<std::shared_ptr<T>, T>(luaState, &std::shared_ptr<T>::get);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[shared_ptr sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[shared_ptr sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


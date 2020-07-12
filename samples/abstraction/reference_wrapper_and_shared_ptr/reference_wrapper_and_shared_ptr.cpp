//
//  reference_wrapper_and_shared_ptr.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2019, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
    void printMessage() const {
        std::cout << "Object " << this << " message!" << std::endl;
    }
};

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["Object"] = integral::ClassMetatable<Object>()
            .setFunction("printMessage", &Object::printMessage);

        // std::reference_wrapper<T> has automatic synthetic inheritance do T as if it was defined as:
        // luaState.defineInheritance([](std::reference_wrapper<T> *referenceWrapperPointer) -> T * {
        //     return &referenceWrapperPointer->get();
        // });

        Object object;
        object.printMessage(); //prints 'Object 0x7ffee8b68560 message!'
        luaState["objectReference"] = std::ref(object);
        luaState.doString("objectReference:printMessage()"); //prints the same previous address 'Object 0x7ffee8b68560 message!'

        // std::shared_ptr<T> has automatic synthetic inheritance do T as if it was defined as:
        // luaState.defineInheritance([](std::shared_ptr<T> *sharedPtrPointer) -> T * {
        //     return sharedPtrPointer->get();
        // });

        std::shared_ptr<Object> objectSharedPtr = std::make_shared<Object>();
        objectSharedPtr->printMessage(); //prints 'Object 0x7fce594077a8 message!'
        luaState["objectSharedPtr"] = objectSharedPtr;
        luaState.doString("objectSharedPtr:printMessage()"); //prints the same previous address 'Object 0x7fce594077a8 message!'
        luaState["objectSharedPtr"].get<Object>().printMessage(); //prints the same previous address 'Object 0x7fce594077a8 message!'

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[reference_wrapper_and_shared_ptr] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

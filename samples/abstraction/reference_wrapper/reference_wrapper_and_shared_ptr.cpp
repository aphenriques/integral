//
//  reference_wrapper_and_shared_ptr.cpp
//  integral
//
//  Copyright (C) 2019  André Pereira Henriques
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

int main(int argc, char* argv[]) {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["Object"] = integral::ClassMetatable<Object>()
            .setFunction("printMessage", &Object::printMessage);

        // 'synthetic inheritance' can be viewed as a transformation from composition in c++ to inheritance in lua
        luaState.defineReferenceWrapperInheritance<Object>();
        // alternative expression:
        //luaState.defineInheritance[](std::reference_wrapper<Object> *referenceWrapperPointer) -> Object * {
        //    return &referenceWrapperPointer->get();
        //});

        Object object;
        object.printMessage(); //prints 'Object 0x7ffee8b68560 message!'
        luaState["objectReference"] = std::ref(object);
        luaState.doString("objectReference:printMessage()"); //prints the same previous address 'Object 0x7ffee8b68560 message!'

        luaState.defineSharedPtrInheritance<Object>();
        // alternative expression:
        //luaState.defineInheritance[](std::shared_ptr<Object> *sharedPtrPointer) -> Object * {
        //    return sharedPtrPointer->get();
        //});

        std::shared_ptr<Object> objectSharedPtr = std::make_shared<Object>();
        objectSharedPtr->printMessage(); //prints 'Object 0x7fce594077a8 message!'
        luaState["objectSharedPtr"] = objectSharedPtr;
        luaState.doString("objectSharedPtr:printMessage()"); //prints the same previous address 'Object 0x7fce594077a8 message!'

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[reference_wrapper_and_shared_ptr] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

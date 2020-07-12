//
//  table.cpp
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

#include <cstdlib>
#include <iostream>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    Object(const std::string &name) : name_(name) {}

    std::string getHello() const {
        return std::string("Hello ") + name_ + '!';
    }

private:
    std::string name_;
};

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["group"] = integral::Table()
                                .set("constant", integral::Table()
                                    .set("pi", 3.14))
                                .setFunction("printHello", []{
                                    std::puts("Hello!");
                                })
                                .set("Object", integral::ClassMetatable<Object>()
                                    .setConstructor<Object(const std::string &)>("new")
                                    .setFunction("getHello", &Object::getHello));
        luaState.doString("print(group.constant.pi)\n"
                          "group.printHello()\n"
                          "print(group.Object.new('object'):getHello())");

        try {
            luaState["group"]["pi"].get<double>();
        } catch (const integral::ReferenceException &referenceException) {
            std::cout << "expected exception: {" << referenceException.what() << "}\n";
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[table] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

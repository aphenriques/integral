//
//  ignored_argument.cpp
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

#include <iostream>
#include <vector>
#include <integral/integral.hpp>

// std::vector<double> is automatically converted to a lua table. Vector is a regular class for integral and is not converted to a lua table
class Vector : public std::vector<double> {};

int main() {
    try {
        integral::State luaState;
        luaState.openLibs();

        luaState["Vector"] = integral::ClassMetatable<Vector>()
                                .setConstructor<Vector()>("new")
                                // because of some legacy lua implementation details, __len receives two arguments, the second argument can be safely ignored
                                .setFunction("__len", [](const Vector &vector, integral::LuaIgnoredArgument) -> std::size_t {
                                    return vector.size();
                                })
                                // explicit cast is necessary to avoid ambiguity
                                .setFunction("pushBack", static_cast<void(Vector::*)(const double &)>(&Vector::push_back));

        luaState.doString("v = Vector.new()\n"
                          "print(#v)\n" // prints "0'
                          "v:pushBack(42)\n"
                          "print(#v)\n" // prints "1"
                          "v:pushBack(42)\n"
                          "print(#v)"); // prints "2"

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[ignored_argument] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

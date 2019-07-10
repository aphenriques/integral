//
//  ignored_argument.cpp
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

#include <iostream>
#include <vector>
#include <integral/integral.hpp>

// std::vector<double> is automatically converted to a lua table. Vector is a regular class for integral and is not converted to a lua table
class Vector : public std::vector<double> {};

int main(int argc, char* argv[]) {
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

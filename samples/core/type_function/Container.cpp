//
//  Container.cpp
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

#include <exception>
#include <functional>
#include <lua.hpp>
#include <integral/integral.hpp>

class Id {
public:
    double number_;
};

class Container {
public:
    Id id_;

    Container(double number) : id_{number} {}
};

extern "C" {
    LUALIB_API int luaopen_libContainer(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Container>(luaState);
            integral::setConstructor<Container(double)>(luaState, "new");

            // type function from Container to Id (similar to C++ type operator)
            integral::defineTypeFunction(luaState, [](Container *container) -> Id * {
                return &container->id_;
            });

            // this function takes an Id object as parameter
            integral::setFunction(luaState, "getNumberFromId", [](const Id &id) -> double {
                return id.number_;
            });

            // type function from Container to double
            integral::defineTypeFunction(luaState, [](Container *container) -> double * {
                return &container->id_.number_;
            });

            // this function takes a double as parameter
            integral::setFunction(luaState, "getNegativeNumber", [](double number) -> double {
                return -number;
            });

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[type_function sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[type_function sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


//
//  Object.cpp
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
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    std::string string_;

    Object(const std::string &string) : string_(string) {}

    void testDefault(int dummy) const {
        std::cout << "Object::string_: \"" << string_ << "\" | dummy argument: " << dummy << std::endl;
    }
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);

            // default argument in constructor
            integral::setConstructor<Object(const std::string &)>(luaState, "new", integral::DefaultArgument<std::string, 1>("default Object.new argument"));

            // DefaultArgument template parameters are checked at compile time. If there is type or index inconsistency, there will be compilation error. E.g:
            // will fail:
            //integral::setConstructor<Object(const std::string &)>(luaState, "new", integral::DefaultArgument<std::string, 2>("default Object.new argument"));
            // will fail:
            //integral::setConstructor<Object(const std::string &)>(luaState, "new", integral::DefaultArgument<double, 1>(42.0));
            // will fail:
            //integral::setConstructor<Object(const std::string &)>(luaState, "new", integral::DefaultArgument<std::string, 1>("default Object.new argument"), integral::DefaultArgument<std::string, 1>("wrong redefinition of default argument"));

            integral::setGetter(luaState, "getString", &Object::string_);

            // default argument in function
            integral::setFunction(luaState, "testDefault", &Object::testDefault, integral::DefaultArgument<Object, 1>("default Object.testDefault Object argument"), integral::DefaultArgument<int, 2>(42));

            integral::setFunction(luaState, "testDefault2", [](double number, const std::string &string) -> void {
                std::cout << "Object.testDefault2 - number: " << number << "; string: \"" << string << "\"" << std::endl;
            }, integral::DefaultArgument<double, 1>(0.42));

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[default_argument sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[default_argument sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


//
//  library.cpp
//  integral
//
// MIT License
//
// Copyright (c) 2015, 2016, 2017, 2020 André Pereira Henriques (aphenriques (at) outlook (dot) com)
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
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    inline Object(const std::string &name) : name_(name) {}
    std::string getName() const {
        return name_;
    }
private:
    const std::string name_;
};

extern "C" {
    LUALIB_API int luaopen_liblibrary(lua_State *luaState) {
        try {
            lua_newtable(luaState);
            // stack: table (module table)

            integral::pushClassMetatable<Object>(luaState);
            integral::setFunction(luaState, "getName", &Object::getName);
            lua_pop(luaState, 1);

            using ObjectConstructorAdaptorType = integral::ConstructorWrapper<Object(const std::string &)>;
            integral::setFunction(luaState, "getObjectConstructor", []() -> ObjectConstructorAdaptorType {
                return ObjectConstructorAdaptorType();
            });

            lua_pushstring(luaState, "getInverse");
            integral::push<integral::FunctionWrapper<double(double)>>(luaState, [](double x) -> double {
                return 1.0/x;
            });
            lua_rawset(luaState, -3);

            integral::setFunction(luaState, "getConstantFunction", [](double constant) -> integral::LuaFunctionWrapper {
                return [constant](lua_State *lambdaLuaState) -> int {
                    integral::push<double>(lambdaLuaState, constant);
                    return 1;
                };
            });

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[adaptors_functions sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[adaptors_functions sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}

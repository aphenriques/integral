//
//  SyntheticallyDerived.cpp
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

#include <functional>
#include <iostream>
#include <string>
#include <lua.hpp>
#include <integral/integral.hpp>

class BaseOfSyntheticBase {
public:
    void baseOfSyntheticBaseMethod() const {
        std::cout << "baseOfSyntheticBaseMethod" << std::endl;
    }
};

class SyntheticBase : public BaseOfSyntheticBase {
public:
    void syntheticBaseMethod() const {
        std::cout << "syntheticBaseMethod" << std::endl;
    }
};

// SyntheticallyDerived does NOT have base classes in c++
class SyntheticallyDerived {
public:
    SyntheticBase syntheticBase_;

    void syntheticallyDerivedMethod() const {
        std::cout << "syntheticallyDerivedMethod" << std::endl;
    }

    SyntheticBase * getSyntheticBasePointer() {
        return &syntheticBase_;
    }
};

SyntheticBase * getSyntheticInheritance(SyntheticallyDerived *syntheticallyDerived) {
    return &syntheticallyDerived->syntheticBase_;
}

extern "C" {
    LUALIB_API int luaopen_libSyntheticallyDerived(lua_State *luaState) {
        try {
            // 'synthetic inheritance' works just like inheritance with a conversion function provided
            // it is like defining type functions, but with methods inheritance

            integral::pushClassMetatable<BaseOfSyntheticBase>(luaState);
            integral::setFunction(luaState, "baseOfSyntheticBaseMethod", &BaseOfSyntheticBase::baseOfSyntheticBaseMethod);
            lua_pop(luaState, 1);

            integral::pushClassMetatable<SyntheticBase>(luaState);
            integral::setFunction(luaState, "syntheticBaseMethod", &SyntheticBase::syntheticBaseMethod);
            lua_pop(luaState, 1);

            // regular inheritance can be mixed with 'synthetic inheritance'
            integral::defineInheritance<SyntheticBase, BaseOfSyntheticBase>(luaState);

            integral::pushClassMetatable<SyntheticallyDerived>(luaState);
            integral::setConstructor<SyntheticallyDerived()>(luaState, "new");
            integral::setFunction(luaState, "syntheticallyDerivedMethod", &SyntheticallyDerived::syntheticallyDerivedMethod);

            // 'synthetic inheritance' can be viewed as a transformation from composition in c++ to inheritance in lua
            // there are alternative ways to define it (just like integral::defineTypeFunction)
            integral::defineInheritance(luaState, [](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
                return &syntheticallyDerived->syntheticBase_;
            });
            // equivalent expression:
            // integral::defineInheritance(luaState, &getSyntheticInheritance);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[synthetic_inheritance sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[synthetic_inheritance sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


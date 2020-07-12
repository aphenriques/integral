//
//  synthetic_inheritance.cpp
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

int main() {
    try {
        // 'synthetic inheritance' works just like inheritance with a conversion function provided
        // it is like defining type functions, but with methods inheritance

        integral::State luaState;

        luaState["BaseOfSyntheticBase"] = integral::ClassMetatable<BaseOfSyntheticBase>()
                                          .setFunction("baseOfSyntheticBaseMethod", &BaseOfSyntheticBase::baseOfSyntheticBaseMethod);

        // regular inheritance can be mixed with 'synthetic inheritance'
        luaState["SyntheticBase"] = integral::ClassMetatable<SyntheticBase>()
                                    .setFunction("syntheticBaseMethod", &SyntheticBase::syntheticBaseMethod)
                                    .setBaseClass<BaseOfSyntheticBase>();

        // 'synthetic inheritance' can be viewed as a transformation from composition in c++ to inheritance in lua
        luaState["SyntheticallyDerived"] = integral::ClassMetatable<SyntheticallyDerived>()
                                           .setConstructor<SyntheticallyDerived()>("new")
                                           .setFunction("syntheticallyDerivedMethod", &SyntheticallyDerived::syntheticallyDerivedMethod)
                                           .setBaseClass([](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
                                               return &syntheticallyDerived->syntheticBase_;
                                           });
        // alternative expression:
        //luaState.defineInheritance([](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
            //return &syntheticallyDerived->syntheticBase_;
        //});

        luaState.doString("syntheticallyDerived = SyntheticallyDerived.new()\n"
                          "syntheticallyDerived:baseOfSyntheticBaseMethod()\n" // prints "baseOfSyntheticBaseMethod"
                          "syntheticallyDerived:syntheticBaseMethod()\n" // prints "syntheticBaseMethod"
                          "syntheticallyDerived:syntheticallyDerivedMethod()"); // prints "syntheticallyDerivedMethod"

        return EXIT_SUCCESS;
    } catch (const std::exception &exception) {
        std::cerr << "[synthetic_inheritance] " << exception.what() << std::endl;
    } catch (...) {
        std::cerr << "unknown exception thrown" << std::endl;
    }
    return EXIT_FAILURE;
}

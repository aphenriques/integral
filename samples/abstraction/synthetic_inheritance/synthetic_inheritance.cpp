//
//  synthetic_inheritance.cpp
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

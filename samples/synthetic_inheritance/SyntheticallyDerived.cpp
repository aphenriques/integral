//
//  SyntheticallyDerived.cpp
//  integral
//
//  Copyright (C) 2014  André Pereira Henriques
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

#include <functional>
#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

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
    void syntheticallyDerivedMethod() const {
        std::cout << "syntheticallyDerivedMethod" << std::endl;
    }

    SyntheticBase * getSyntheticBasePointer() {
        return &syntheticBase_;
    }

    SyntheticBase syntheticBase_;
};

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
            integral::setConstructor<SyntheticallyDerived>(luaState, "new");
            integral::setFunction(luaState, "syntheticallyDerivedMethod", &SyntheticallyDerived::syntheticallyDerivedMethod);

            // 'synthetic inheritance' can be viewed as a transformation from composition in c++ to inheritance in lua
            // there are alternative ways to define it (just like integral::defineTypeFunction)
            integral::defineInheritance<SyntheticallyDerived, SyntheticBase>(luaState, [](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
                return &syntheticallyDerived->syntheticBase_;
            });

            // alternative expressions:
            //integral::defineInheritance<SyntheticallyDerived, SyntheticBase>(luaState, &SyntheticallyDerived::getSyntheticBasePointer);
            //integral::defineInheritance(luaState, &SyntheticallyDerived::syntheticBase_);
            //integral::defineInheritance(luaState, std::function<SyntheticBase *(SyntheticallyDerived *)>([](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
                //return &syntheticallyDerived->syntheticBase_;
            //}));

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


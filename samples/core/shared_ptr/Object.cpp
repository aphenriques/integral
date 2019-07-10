//
//  Object.cpp
//  integral
//
//  Copyright (C) 2014, 2015, 2016, 2017  André Pereira Henriques
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
#include <functional>
#include <memory>
#include <lua.hpp>
#include <integral/integral.hpp>

class Object {
public:
    void print() const {
        std::cout << "Object::print" << std::endl;
    }
};

extern "C" {
    LUALIB_API int luaopen_libObject(lua_State *luaState) {
        try {
            integral::pushClassMetatable<Object>(luaState);
            integral::setFunction(luaState, "print", &Object::print);
            integral::setFunction(luaState, "getShared", &std::make_shared<Object>);

            // std::shared_ptr<T> has automatic synthetic inheritance do T as if it was defined as:
            // integral::defineInheritance(luaState, [](std::shared_ptr<T> *sharedPtrPointer) -> T * {
            //     return sharedPtrPointer->get();
            // });
            // or:
            // integral::defineInheritance(luaState, std::function<T *(std::shared_ptr<T> *)>(&std::shared_ptr<T>::get));
            // the following statement may not work if the get method is from a base class of std::shared_ptr (may fail in some stdlib implementations)
            // integral::defineInheritance<std::shared_ptr<T>, T>(luaState, &std::shared_ptr<T>::get);

            return 1;
        } catch (const std::exception &exception) {
            lua_pushstring(luaState, (std::string("[shared_ptr sample setup] ") + exception.what()).c_str());
        } catch (...) {
            lua_pushstring(luaState, "[shared_ptr sample setup] unknown exception thrown");
        }
        // Error return outside catch scope so that the exception destructor can be called
        return lua_error(luaState);
    }
}


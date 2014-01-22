//
//  core.h
//  integral
//
//  Copyright (C) 2013  André Pereira Henriques
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

#ifndef integral_core_h
#define integral_core_h

#include <string>
#include <functional>
#include <utility>
#include <lua.hpp>
#include "FunctionWrapper.h"
#include "LuaFunctionWrapper.h"
#include "exchanger.h"
#include "type_manager.h"
#include "constructor.h"

namespace integral {
    namespace core {
        // Pushes class metatable of type "T".
        // The class will be registered in the first time this function is called.
        // If the class is already registered, the registered class metatable is pushed normally.
        template<typename T>
        inline void pushClassMetatable(lua_State *luaState);
        
        // Pushes a type "T" value (string or number) or object onto the stack.
        // References and pointers (except char *) can not be pushed.
        // The class metatable is automatically registered if needed.
        // "arguments": parameters for value initialization or object construction.
        template<typename T, typename ...A>
        inline void push(lua_State *luaState, A &&...arguments);
        
        // Returns a type "T" value (string or number) or object from the stack at "index" position.
        // Objects are returned as references.
        // If the type is incorrect, an exception is thrown.
        template<typename T>
        inline auto get(lua_State *luaState, int index) -> decltype(exchanger::get<T>(luaState, index));
        
        // Sets a constructor function in the table or metatable on top of the stack.
        // "typename T": type of the class of the constructor.
        // "typename ...A": list of constructor argument types.
        // "name": name of the bound Lua function.
        template<typename T, typename ...A>
        inline void setConstructor(lua_State *luaState, const std::string &name);
        
        // Sets a lua_CFunction style function in the table or metatable on top of the stack.
        // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
        // "name": name of the bound Lua function.
        // "function": function that takes a lua_State * as argument and return an int (number of return values/objects).
        // "nUpValues": number of upvalues on the stack.
        inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function, int nUpValues = 0);
        
        inline void setLuaFunction(lua_State *luaState, const std::string &name, const std::function<int(lua_State *)> &function, int nUpValues = 0);
        
        template<typename T>
        inline void setLuaFunction(lua_State *luaState, const std::string &name, const T &&function, int nUpValues = 0);
        
        // Binds a function in the table or metatable on top of the stack.
        // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
        // Functions that return reference or pointer (except char *) will cause compilation error.
        // Returnning references and pointers in bound functions is regarded as unsafe, therefore not supported.
        // "name": name of the bound Lua function.
        // "function": function to be bound
        // "nUpValues": number of upvalues on the stack.
        template<typename R, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function);
        
        template<typename R, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...));
        
        template<typename R, typename T, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...));
        
        template<typename R, typename T, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const);
        
        // Binds a getter function in the table or metatable on top of the stack.
        // The function returns by value (_not_ by reference)
        // "name": name of the bound Lua function.
        // "attribute": attribute address.
        template<typename R, typename T>
        void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute);
        
        // Binds a setter function in the table or metatable on top of the stack.
        // "name": name of the bound Lua function.
        // "attribute": attribute address.
        template<typename R, typename T>
        void setSetter(lua_State *luaState, const std::string &name, R T::* attribute);
        
        //--
        
        template<typename T>
        inline void pushClassMetatable(lua_State *luaState) {
            type_manager::pushClassMetatable<T>(luaState);
        }
        
        template<typename T, typename ...A>
        inline void push(lua_State *luaState, A &&...arguments) {
            exchanger::push<T>(luaState, std::forward<A>(arguments)...);
        }
        
        template<typename T>
        inline auto get(lua_State *luaState, int index) -> decltype(exchanger::get<T>(luaState, index)) {
            return exchanger::get<T>(luaState, index);
        }
        
        template<typename T, typename ...A>
        inline void setConstructor(lua_State *luaState, const std::string &name) {
            LuaFunctionWrapper::setFunction(luaState, name, &constructor::callConstructor<T, A...>);
        }
        
        inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function, int nUpValues) {
            LuaFunctionWrapper::setFunction(luaState, name, function, nUpValues);
        }
        
        inline void setLuaFunction(lua_State *luaState, const std::string &name, const std::function<int(lua_State *)> &function, int nUpValues) {
            LuaFunctionWrapper::setFunction(luaState, name, function, nUpValues);
        }
        
        template<typename T>
        inline void setLuaFunction(lua_State *luaState, const std::string &name, const T &&function, int nUpValues) {
            LuaFunctionWrapper::setFunction(luaState, name,  std::function<int(lua_State *)>(std::forward<const T>(function)), nUpValues);
            
        }
        
        template<typename R, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function) {
            FunctionWrapper<R, A...>::setFunction(luaState, name, function);
        }
        
        template<typename R, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...)) {
            setFunction(luaState, name, std::function<R(A...)>(function));

        }
        
        template<typename R, typename T, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...)) {
            setFunction(luaState, name, std::function<R(T &, A...)>(function));
        }
        
        template<typename R, typename T, typename ...A>
        inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const) {
            setFunction(luaState, name, std::function<R(const T &, A...)>(function));
        }
        
        template<typename R, typename T>
        void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute) {
            setFunction(luaState, name, std::function<R(const T &)>([attribute](const T &object) -> R {
                return object.*attribute;
            }));
        }
        
        template<typename R, typename T>
        void setSetter(lua_State *luaState, const std::string &name, R T::* attribute) {
            setFunction(luaState, name, std::function<void(T &, const R&)>([attribute](T &object, const R &value) -> void {
                object.*attribute = value;
            }));
        }
    }
}

#endif

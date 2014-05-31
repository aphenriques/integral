//
//  core.h
//  integral
//
//  Copyright (C) 2013, 2014  André Pereira Henriques
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

#include <functional>
#include <string>
#include <typeindex>
#include <utility>
#include <lua.hpp>
#include "ConstructorWrapper.h"
#include "DefaultArgument.h"
#include "DefaultArgumentManager.h"
#include "exchanger.h"
#include "FunctionWrapper.h"
#include "LuaFunctionWrapper.h"
#include "type_manager.h"

namespace integral {
    // Adaptor to std::vector<T> and lua table.
    // LuaVector can be seamlessly converted to std::vector.
    template<typename T>
    using LuaVector = detail::exchanger::LuaVector<T>;
    
    // Adaptor to std::array<T, N> and lua table.
    // LuaArray can be seamlessly converted to std::array.
    template<typename T, std::size_t N>
    using LuaArray = detail::exchanger::LuaArray<T, N>;
    
    // Adaptor to std::tuple<T...> and lua pack (multiple return values).
    // It is used to push multiple (function return) values.
    // LuaPack can be seamlessly converted to std::tuple.
    template<typename ...T>
    using LuaPack = detail::exchanger::LuaPack<T...>;
    
    // Proxy to a (either C or lua) function in lua state.
    // The object of this class cannot be stored, it only points to a function in the stack.
    // It is meant to be used as an argument to a C function.
    template<typename R, typename ...A>
    using LuaFunctionArgument = detail::exchanger::LuaFunctionArgument<R, A...>;
    
    // Pushes class metatable of type "T".
    // The class metatable can be converted to base types EVEN when they are NOT especified with defineTypeFunction or defineInheritance.
    // The class will be registered in the first time this function is called.
    // If the class is already registered, the registered class metatable is pushed normally.
    template<typename T>
    inline void pushClassMetatable(lua_State *luaState);
    
    // Sets a type conversion function of derived class D to base class B.
    // typename D class metatable need not be on the stack
    // The conversion to base types still happens even if this function is not used
    // This function is mostly useful when using 'synthetic' inheritance
    // Methods are not inherited with this function.
    // If D -> B is not a possible conversion, there will be a compilation error.
    template<typename D, typename B>
    inline void defineTypeFunction(lua_State *luaState);
    
    // Sets a type conversion function of class T to class U.
    // typename T class metatable need not be on the stack
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
    
    // Sets a type conversion function of class T to class U.
    // typename T class metatable need not be on the stack
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(*typeFunction)(T *));
    
    // Sets a type conversion function of class T to class U.
    // typename T class metatable need not be on the stack
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(T::*typeFunction)());
    
    // Sets a type conversion function of class T to class U.
    // typename T class metatable need not be on the stack
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    void defineTypeFunction(lua_State *luaState, U T::* attribute);
    
    // Sets inheritance between derived class D to base class B.
    // typename D class metatable need not be on the stack
    // Methods are inherited with this function.
    // If D -> B is not a possible conversion, there will be a compilation error.
    template<typename D, typename B>
    inline void defineInheritance(lua_State *luaState);
    
    // Sets 'synthetic' inheritance between class T to class U using typeFunction conversion function.
    // typename T class metatable need not be on the stack
    // Methods are inherited with this function.
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
    
    // Sets 'synthetic' inheritance between class T to class U using typeFunction conversion function.
    // typename T class metatable need not be on the stack
    // Methods are inherited with this function.
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(*typeFunction)(T *));
    
    // Sets 'synthetic' inheritance between class T to class U using typeFunction conversion function.
    // typename T class metatable need not be on the stack
    // Methods are inherited with this function.
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(T::*typeFunction)());
    
    // Sets 'synthetic' inheritance between class T to class U using member attribute pointer.
    // typename T class metatable need not be on the stack
    // Methods are inherited with this function.
    // Class U can be any class, not necessarily a base class
    template<typename T, typename U>
    void defineInheritance(lua_State *luaState, U T::* attribute);
    
    // Pushes a type "T" value (string or number) or object onto the stack.
    // References and pointers (except char *) can not be pushed.
    // The class metatable is automatically registered if needed.
    // "arguments": parameters for value initialization or object construction.
    template<typename T, typename ...A>
    inline void push(lua_State *luaState, A &&...arguments);
    
    // Returns a type "T" value (string or number) or object from the stack at "index" position.
    // Objects are returned by reference.
    // Lua types (number, table and strings) are returned by value.
    // If the type is incorrect, an exception is thrown.
    template<typename T>
    inline auto get(lua_State *luaState, int index) -> decltype(detail::exchanger::get<T>(luaState, index));
    
    // Sets a constructor function in the table or metatable on top of the stack.
    // "typename T": type of the class of the constructor.
    // "typename ...A": list of constructor argument types.
    // "name": name of the bound Lua function.
    // "defaultArguments...": pack of default arguments. Each with its own specific type E at index I (every type is deduced from the function arguments). DefaultArgument Index I starts with 1 (like lua). Static checking is performed, so the invalid type or/and index causes compilation error. DefaultArgument<E, I> constructor arguments are forwarded for T type object constructor (check DefaultConstructor.h for details)
    // defaultArguments... argument is a rvalue in order to utilize move constructors whenever possible. Moreover, forcing the declaration of DefaultArgument in-place adds clarity.
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments);
    
    // Sets a lua_CFunction style function in the table or metatable on top of the stack.
    // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
    // "name": name of the bound Lua function.
    // "function": function that takes a lua_State * as argument and return an int (number of return values/objects).
    // no upvalues because function objects can be used
    inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function);
    
    inline void setLuaFunction(lua_State *luaState, const std::string &name, const std::function<int(lua_State *)> &function);
    
    template<typename T>
    inline void setLuaFunction(lua_State *luaState, const std::string &name, T &&function);
    
    // Binds a function in the table or metatable on top of the stack.
    // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
    // Functions that return reference or pointer (except char *) will cause compilation error.
    // Returnning references and pointers in bound functions is regarded as unsafe, therefore not supported.
    // "name": name of the bound Lua function.
    // "function": function to be bound
    // "defaultArguments...": pack of default arguments. Each with its own specific type E at index I (every type is deduced from the function arguments). DefaultArgument Index I starts with 1 (like lua). Static checking is performed, so the invalid type or/and index causes compilation error. DefaultArgument<E, I> constructor arguments are forwarded for T type object constructor (check DefaultConstructor.h for details)
    // defaultArguments... argument is a rvalue in order to utilize move constructors whenever possible. Moreover, forcing the declaration of DefaultArgument in-place adds clarity.
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments);
    
    // Binds a getter function in the table or metatable on top of the stack.
    // The function returns by value (_not_ by reference)
    // "name": name of the bound Lua function.
    // "attribute": attribute address.
    template<typename R, typename T>
    inline void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute);
    
    // Binds a setter function in the table or metatable on top of the stack.
    // "name": name of the bound Lua function.
    // "attribute": attribute address.
    template<typename R, typename T>
    inline void setSetter(lua_State *luaState, const std::string &name, R T::* attribute);
    
    // Calls function on top of the stack
    // "arguments" are pushed by value.
    // "R" is the return type, returned as follows (such as integral::get):
    // - objects are returned by reference;
    // - lua types (number, table and strings) are returned by value.
    // The function is popped from stack
    // Throws a CallerException exception on error.
    // Attention! Explicitly specifying argument types causes compilation error on Clang (it works on GCC). It looks like it is a bug.
    // It is not necessary to explicitly specify argument types.
    template<typename R, typename ...A>
    inline auto call(lua_State *luaState, A &&...arguments) -> decltype(detail::exchanger::Caller<R, A...>::call(luaState, std::forward<A>(arguments)...));
    
    //--
    
    template<typename T>
    inline void pushClassMetatable(lua_State *luaState) {
        detail::type_manager::pushClassMetatable<T>(luaState);
    }
    
    template<typename D, typename B>
    inline void defineTypeFunction(lua_State *luaState) {
        detail::type_manager::defineTypeFunction<D, B>(luaState);
    }
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
        detail::type_manager::defineTypeFunction(luaState, typeFunction);
    }
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(*typeFunction)(T *)) {
        defineTypeFunction(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(T::*typeFunction)()) {
        defineTypeFunction(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T, typename U>
    void defineTypeFunction(lua_State *luaState, U T::* attribute) {
        defineTypeFunction(luaState, std::function<U *(T *)>([attribute](T * t) -> U * {
            return &(t->*attribute);
        }));
    }
    
    template<typename D, typename B>
    inline void defineInheritance(lua_State *luaState) {
        detail::type_manager::defineInheritance<D, B>(luaState);
    }
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction) {
        detail::type_manager::defineInheritance(luaState, typeFunction);
    }
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(*typeFunction)(T *)) {
        defineInheritance(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(T::*typeFunction)()) {
        defineInheritance(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T, typename U>
    void defineInheritance(lua_State *luaState, U T::* attribute) {
        defineInheritance(luaState, std::function<U *(T *)>([attribute](T * t) -> U * {
            return &(t->*attribute);
        }));
    }
    
    template<typename T, typename ...A>
    inline void push(lua_State *luaState, A &&...arguments) {
        detail::exchanger::push<T>(luaState, std::forward<A>(arguments)...);
    }
    
    template<typename T>
    inline auto get(lua_State *luaState, int index) -> decltype(detail::exchanger::get<T>(luaState, index)) {
        return detail::exchanger::get<T>(luaState, index);
    }
    
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments) {
        detail::ConstructorWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, T, A...>::setConstructor(luaState, name, std::forward<DefaultArgument<E, I>>(defaultArguments)...);
    }
    
    inline void setLuaFunction(lua_State *luaState, const std::string &name, lua_CFunction function) {
        detail::LuaFunctionWrapper::setFunction(luaState, name, function);
    }
    
    inline void setLuaFunction(lua_State *luaState, const std::string &name, const std::function<int(lua_State *)> &function) {
        detail::LuaFunctionWrapper::setFunction(luaState, name, function);
    }
    
    template<typename T>
    inline void setLuaFunction(lua_State *luaState, const std::string &name, T &&function) {
        // explicit call to detail::LuaFunctionWrapper::setFunction is necessary to avoid infinite recursion
        detail::LuaFunctionWrapper::setFunction(luaState, name,  std::function<int(lua_State *)>(std::forward<const T>(function)));
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments) {
        detail::FunctionWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, R, A...>::setFunction(luaState, name, function, std::forward<DefaultArgument<E, I>>(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(A...)>(function), std::forward<DefaultArgument<E, I>>(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(T &, A...)>(function), std::forward<DefaultArgument<E, I>>(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(const T &, A...)>(function), std::forward<DefaultArgument<E, I>>(defaultArguments)...);
    }
    
    template<typename R, typename T>
    inline void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute) {
        setFunction(luaState, name, std::function<R(const T &)>([attribute](const T &object) -> R {
            return object.*attribute;
        }));
    }
    
    template<typename R, typename T>
    inline void setSetter(lua_State *luaState, const std::string &name, R T::* attribute) {
        setFunction(luaState, name, std::function<void(T &, const R&)>([attribute](T &object, const R &value) -> void {
            object.*attribute = value;
        }));
    }
    
    template<typename R, typename ...A>
    inline auto call(lua_State *luaState, A &&...arguments) -> decltype(detail::exchanger::Caller<R, A...>::call(luaState, std::forward<A>(arguments)...)) {
        return detail::exchanger::Caller<R, A...>::call(luaState, std::forward<A>(arguments)...);
    }
}

#endif

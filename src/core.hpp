//
//  core.hpp
//  integral
//
//  Copyright (C) 2013, 2014, 2015, 2016  André Pereira Henriques
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

#ifndef integral_core_hpp
#define integral_core_hpp

#include <functional>
#include <string>
#include <typeindex>
#include <utility>
#include <lua.hpp>
#include "Caller.hpp"
#include "CConstructor.hpp"
#include "CFunction.hpp"
#include "ConstructorWrapper.hpp"
#include "DefaultArgument.hpp"
#include "DefaultArgumentManager.hpp"
#include "exchanger.hpp"
#include "FunctionWrapper.hpp"
#include "FunctorTypeFinder.hpp"
#include "LuaFunctionArgument.hpp"
#include "LuaFunctionWrapper.hpp"
#include "LuaIgnoredArgument.hpp"
#include "LuaPack.hpp"
#include "message.hpp"
#include "type_manager.hpp"

namespace integral {
    // Exception thrown by Caller
    using CallerException = detail::CallerException;
    
    // Proxy to any value in lua state.
    // It is meant to be used as an argument to a C++ function.
    using LuaIgnoredArgument = detail::LuaIgnoredArgument;
    
    // Adaptor to std::vector<T> and lua table.
    // LuaVector can be seamlessly converted to std::vector.
    template<typename T>
    using LuaVector = detail::exchanger::LuaVector<T>;
    
    // Adaptor to std::array<T, N> and lua table.
    // LuaArray can be seamlessly converted to std::array.
    template<typename T, std::size_t N>
    using LuaArray = detail::exchanger::LuaArray<T, N>;
    
    // Adaptor to std::unordered_map<T, U> and lua table.
    // LuaUnorderedMap can be seamlessly converted to std::unordered_map.
    template<typename T, typename U>
    using LuaUnorderedMap = detail::exchanger::LuaUnorderedMap<T, U>;
    
    // Adaptor to std::tuple<T...> and lua table.
    // LuaTuple can be seamlessly converted to std::tuple.
    template<typename ...T>
    using LuaTuple = detail::exchanger::LuaTuple<T...>;
    
    // Adaptor to std::tuple<T...> and lua pack (multiple return values).
    // It is used to push multiple (function return) values.
    // LuaPack can be seamlessly converted to std::tuple.
    template<typename ...T>
    using LuaPack = detail::LuaPack<T...>;
    
    //lua_CFunction like function or functor wrapper
    using LuaFunctionWrapper = detail::LuaFunctionWrapper;

    // Proxy to a (either C++ or lua) function in lua state.
    // The object of this class cannot be stored, it only points to a function in the stack.
    // It is meant to be used as an argument to a C++ function.
    // To call the function:
    // - R LuaFunctionArgument::call<R>(const A ...&);
    // - 'R' is a non-reference value (it can be void).
    using LuaFunctionArgument = detail::LuaFunctionArgument;
    
    // Proxy to std::function<T>
    // It is used to push a function onto the lua stack
    // CFuntion can not be gotten with integral::get
    template<typename T>
    using CFunction = detail::CFunction<T>;
    
    // Proxy to class contructor
    // It is used to push a constructor onto the lua stack
    // CConstructor can not be gotten with integral::get
    template<typename T, typename ...A>
    using CConstructor = detail::CConstructor<T, A...>;
    
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
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, std::function<U *(T *)> &&typeFunction);
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(*typeFunction)(T *));
    
    template<typename T>
    inline void defineTypeFunction(lua_State *luaState, T &&typeFunction);
    
    // Sets inheritance between derived class D to base class B.
    // typename D class metatable need not be on the stack
    // Methods are inherited with this function.
    // If D -> B is not a possible conversion, there will be a compilation error.
    // the __index metamethod of the metatable of the class metatable if overriden
    template<typename D, typename B>
    inline void defineInheritance(lua_State *luaState);
    
    // Sets 'synthetic' inheritance between class T to class U using typeFunction conversion function.
    // typename T class metatable need not be on the stack
    // Methods are inherited with this function.
    // Class U can be any class, not necessarily a base class
    // the __index metamethod of the metatable of the class metatable if overriden
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, const std::function<U *(T *)> &typeFunction);
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, std::function<U *(T *)> &&typeFunction);
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(*typeFunction)(T *));
    
    template<typename T>
    inline void defineInheritance(lua_State *luaState, T &&typeFunction);
    
    // Pushes a type "T" value (string or number) or object onto the stack.
    // References and pointers (except const char *) can not be pushed.
    // The class metatable is automatically registered if needed.
    // "arguments": parameters for value initialization or object construction.
    template<typename T, typename ...A>
    inline void push(lua_State *luaState, A &&...arguments);
    
    // Returns a type "T" value (string or number) or object from the stack at "index" position.
    // Objects are returned by reference.
    // Lua types (number, table and strings) are returned by value.
    // If the type is incorrect, an ArgumentException is thrown.
    template<typename T>
    inline decltype(auto) get(lua_State *luaState, int index);
    
    // Sets a constructor function in the table or metatable on top of the stack.
    // "typename T": type of the class of the constructor.
    // "typename ...A": list of constructor argument types.
    // "name": name of the bound Lua function.
    // "defaultArguments...": pack of default arguments. Each with its own specific type E at index I (every type is deduced from the function arguments). DefaultArgument Index I starts with 1 (like lua). Static checking is performed, so the invalid type or/and index causes compilation error. DefaultArgument<E, I> constructor arguments are forwarded for T type object constructor (check DefaultConstructor.h for details)
    // defaultArguments... argument is a rvalue in order to utilize move constructors whenever possible. Moreover, forcing the declaration of DefaultArgument in-place adds clarity.
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushConstructor(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments);
    
    // Sets a lua_CFunction style function in the table or metatable on top of the stack.
    // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
    // "name": name of the bound Lua function.
    // "function": function that takes a lua_State * as argument and return an int (number of return values/objects).
    // The first upvalue in the luacfunction is always the userdata that holds the function (std::function), so the remaining upvalues indexes are offset by 1.
    // Use LuaFunctionWrapper::getUpValueIndex or lua_upvalueindex(index + 1) to get an upvalue index.
    template<typename T>
    inline void setLuaFunction(lua_State *luaState, const std::string &name, T&& luaFunction, int nUpValues = 0);

    template<typename T>
    inline void pushLuaFunction(lua_State *luaState, T&& luaFunction, int nUpValues = 0);

    
    // Binds a function in the table or metatable on top of the stack.
    // The function is managed by integral so that if an exception is thrown from it, it is translated to a Lua error
    // Functions that return reference or pointer (except const char *) will cause compilation error.
    // Returning references and pointers in bound functions is regarded as unsafe, therefore not supported.
    // "name": name of the bound Lua function.
    // "function": function to be bound
    // "defaultArguments...": pack of default arguments. Each with its own specific type E at index I (every type is deduced from the function arguments). DefaultArgument Index I starts with 1 (like lua). Static checking is performed, so the invalid type or/and index causes compilation error. DefaultArgument<E, I> constructor arguments are forwarded for T type object constructor (check DefaultConstructor.h for details)
    // defaultArguments... argument is a rvalue in order to utilize move constructors whenever possible. Moreover, forcing the declaration of DefaultArgument in-place adds clarity.
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, std::function<R(A...)> &&function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename T, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, T &&function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, std::function<R(A...)> &&function, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments);
    
    template<typename T, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, T &&function, DefaultArgument<E, I> &&...defaultArguments);
    
    // Binds a getter function in the table or metatable on top of the stack.
    // The function returns by value (_not_ by reference)
    // "name": name of the bound Lua function.
    // "attribute": attribute address.
    template<typename R, typename T>
    inline void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute);
    
    template<typename R, typename T>
    inline void pushCopyGetter(lua_State *luaState, R T::* attribute);
    
    // Binds a setter function in the table or metatable on top of the stack.
    // "name": name of the bound Lua function.
    // "attribute": attribute address.
    template<typename R, typename T>
    inline void setSetter(lua_State *luaState, const std::string &name, R T::* attribute);
    
    template<typename R, typename T>
    inline void pushSetter(lua_State *luaState, R T::* attribute);
    
    // Calls function on top of the stack
    // "arguments" are pushed by value.
    // "R" is the return type, returned as follows (such as integral::get):
    // - objects are returned by reference;
    // - lua types (number, table and strings) are returned by value.
    // The function is popped from stack
    // Throws a CallerException exception on error.
    // It is not necessary to explicitly specify argument template types.
    template<typename R, typename ...A>
    inline decltype(auto) call(lua_State *luaState, const A &...arguments);
    
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
    inline void defineTypeFunction(lua_State *luaState, std::function<U *(T *)> &&typeFunction) {
        detail::type_manager::defineTypeFunction(luaState, std::move(typeFunction));
    }
    
    template<typename T, typename U>
    inline void defineTypeFunction(lua_State *luaState, U *(*typeFunction)(T *)) {
        detail::type_manager::defineTypeFunction(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T>
    inline void defineTypeFunction(lua_State *luaState, T &&typeFunction) {
        detail::type_manager::defineTypeFunction(luaState, std::function<typename detail::FunctorTypeFinder<T>::FunctionType>(std::forward<T>(typeFunction)));
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
    inline void defineInheritance(lua_State *luaState, std::function<U *(T *)> &&typeFunction) {
        detail::type_manager::defineInheritance(luaState, std::move(typeFunction));
    }
    
    template<typename T, typename U>
    inline void defineInheritance(lua_State *luaState, U *(*typeFunction)(T *)) {
        detail::type_manager::defineInheritance(luaState, std::function<U *(T *)>(typeFunction));
    }
    
    template<typename T>
    inline void defineInheritance(lua_State *luaState, T &&typeFunction) {
        detail::type_manager::defineInheritance(luaState, std::function<typename detail::FunctorTypeFinder<T>::FunctionType>(std::forward<T>(typeFunction)));
    }
    
    template<typename T, typename ...A>
    inline void push(lua_State *luaState, A &&...arguments) {
        detail::exchanger::push<T>(luaState, std::forward<A>(arguments)...);
    }
    
    template<typename T>
    inline decltype(auto) get(lua_State *luaState, int index) {
        return detail::exchanger::get<T>(luaState, index);
    }
    
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setConstructor(lua_State *luaState, const std::string &name, DefaultArgument<E, I> &&...defaultArguments) {
        detail::ConstructorWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, T, A...>::setConstructor(luaState, name, std::move(defaultArguments)...);
    }
    
    template<typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushConstructor(lua_State *luaState, DefaultArgument<E, I> &&...defaultArguments) {
        detail::ConstructorWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, T, A...>::pushConstructor(luaState, std::move(defaultArguments)...);
    }
    
    template<typename T>
    inline void setLuaFunction(lua_State *luaState, const std::string &name, T&& luaFunction, int nUpValues) {
        detail::exchanger::setLuaFunctionWrapper(luaState, name, std::forward<T>(luaFunction), nUpValues);
    }
    
    template<typename T>
    inline void pushLuaFunction(lua_State *luaState, T&& luaFunction, int nUpValues) {
        push<LuaFunctionWrapper>(luaState, std::forward<T>(luaFunction), nUpValues);
    }

    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments) {
        detail::FunctionWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, R, A...>::setFunction(luaState, name, function, std::move(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, std::function<R(A...)> &&function, DefaultArgument<E, I> &&...defaultArguments) {
        detail::FunctionWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, R, A...>::setFunction(luaState, name, std::move(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(T &, A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<R(const T &, A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename T, typename ...E, unsigned ...I>
    inline void setFunction(lua_State *luaState, const std::string &name, T &&function, DefaultArgument<E, I> &&...defaultArguments) {
        setFunction(luaState, name, std::function<typename detail::FunctorTypeFinder<T>::FunctionType>(std::forward<T>(function)), std::move(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, const std::function<R(A...)> &function, DefaultArgument<E, I> &&...defaultArguments) {
        detail::FunctionWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, R, A...>::pushFunction(luaState, function, std::move(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, std::function<R(A...)> &&function, DefaultArgument<E, I> &&...defaultArguments) {
        detail::FunctionWrapper<detail::DefaultArgumentManager<DefaultArgument<E, I>...>, R, A...>::pushFunction(luaState, std::move(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        pushFunction(luaState, std::function<R(A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(T::*function)(A...), DefaultArgument<E, I> &&...defaultArguments) {
        pushFunction(luaState, std::function<R(T &, A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename R, typename T, typename ...A, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, R(T::*function)(A...) const, DefaultArgument<E, I> &&...defaultArguments) {
        pushFunction(luaState, std::function<R(const T &, A...)>(function), std::move(defaultArguments)...);
    }
    
    template<typename T, typename ...E, unsigned ...I>
    inline void pushFunction(lua_State *luaState, T &&function, DefaultArgument<E, I> &&...defaultArguments) {
        pushFunction(luaState, std::function<typename detail::FunctorTypeFinder<T>::FunctionType>(std::forward<T>(function)), std::move(defaultArguments)...);
    }
    
    template<typename R, typename T>
    inline void setCopyGetter(lua_State *luaState, const std::string &name, R T::* attribute) {
        setFunction(luaState, name, std::function<R(const T &)>([attribute](const T &object) -> R {
            return object.*attribute;
        }));
    }
    
    template<typename R, typename T>
    inline void pushCopyGetter(lua_State *luaState, R T::* attribute) {
        pushFunction(luaState, std::function<R(const T &)>([attribute](const T &object) -> R {
            return object.*attribute;
        }));
    }
    
    template<typename R, typename T>
    inline void setSetter(lua_State *luaState, const std::string &name, R T::* attribute) {
        setFunction(luaState, name, std::function<void(T &, const R&)>([attribute](T &object, const R &value) -> void {
            object.*attribute = value;
        }));
    }
    
    template<typename R, typename T>
    inline void pushSetter(lua_State *luaState, R T::* attribute) {
        pushFunction(luaState, std::function<void(T &, const R&)>([attribute](T &object, const R &value) -> void {
            object.*attribute = value;
        }));
    }
    
    template<typename R, typename ...A>
    inline decltype(auto) call(lua_State *luaState, const A &...arguments) {
        return detail::Caller<R, A...>::call(luaState, arguments...);
    }
}

#endif

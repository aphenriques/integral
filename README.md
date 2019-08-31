# integral

`integral` is a C++ library for binding C++ code with Lua (Lua 5.1, Lua 5.2, Lua 5.3 or LuaJIT).

![Lua logo](http://www.lua.org/images/powered-by-lua.gif)

# Index

* [Features](#features)
* [Error safety](#error-safety)
* [Requirements](#requirements)
* [Tested environments](#tested-environments)
* [Build](#build)
* [Usage](#usage)
  * [Create Lua state](#create-lua-state)
  * [Use existing Lua state](#use-existing-lua-state)
  * [Get and set value](#get-and-set-value)
  * [Register function](#register-function)
  * [Register function with default arguments](#register-function-with-default-arguments)
  * [Register class](#register-class)
  * [Get object](#get-object)
  * [Register inheritance](#register-inheritance)
  * [Register table](#register-table)
  * [Use polymorphism](#use-polymorphism)
  * [Call function in Lua state](#call-function-in-lua-state)
  * [Register lua function argument](#register-lua-function-argument)
  * [Table conversion](#table-conversion)
  * [Register function with ignored argument](#register-function-with-ignored-argument)
  * [Optional](#optional)
  * [Register synthetic inheritance](#register-synthetic-inheritance)
  * [std::reference_wrapper and std::shared_ptr automatic inheritance](#stdreference_wrapper-and-stdshared_ptr-automatic-inheritance)
* [Automatic conversion](#automatic-conversion)
* [Automatic inheritance](#automatic-inheritance)
* [integral reserved names in Lua](#integral-reserved-names-in-lua)
* [Source](#source)
* [Author](#author)
* [License](#license)


# Features

* no macros;
* no dependencies; and
* thread safety (as per Lua state): `integral` binds to different Lua states independently.


# Error safety

* `integral` will not crash the Lua state;
* stack unwinding is done before the Lua error handling gets in action;
* thrown exceptions in exported functions are converted to Lua errors;
* wrong parameter types in exported functions turn into Lua errors;
* wrong number of parameters in exported functions turn into Lua errors;
* clear error messages;
* it is not possible (it will cause compilation error) to push to Lua:
  * pointers `T *` (except `const char *` which is considered a string);
  * non-const references `T &` (`const T &` is valid and it is pushed by value);
  * functions returning pointers (except `const char *`) and non-const references
* invalid default arguments definition causes compilation error.


# Requirements

* C++17 compiler; and
* Lua 5.1 or later.


# Tested environments

* gcc version 8.3.0 (Ubuntu 8.3.0-6ubuntu1) on Ubuntu Linux; and
* Apple LLVM version 10.0.0 (clang-1000.11.45.5) on MacOS.


# Build

Grab the `dependecies/exception/include/exception` directory and all the source files (*.hpp and *.cpp) in the `lib/integral` directory and build (there is no preprocessor configuration for the library).

Alternatively, build and install the library with:

    $ make
    $ make install

To build the library, the test or the samples with LuaJIT, use:

    $ make WITH_LUAJIT=y

Lua and LuaJIT compiler settings can be defined with:

    $ make LUA_INCLUDE_DIR=/path/to/lua[jit]/include LUA_LIB_DIR=-L/path/to/lua[jit]/lib LUA_LDLIB=-llua[jit]library [WITH_LUAJIT=y]


# Usage

Include the library header `integral/integral.hpp` (`namespace integral`) and link to `libintegral.a` or `libintegral.so` if the library was built separately.

Check [samples/abstraction](samples/abstraction) directory for examples.

## Create Lua state

```cpp
#include <integral/integral.hpp>

int main(int argc, char * argv[]) {
    integral::State luaState;
    luaState.openLibs();
    luaState.doFile("path/to/script.lua"); // executes lua script
    luaState.doString("print('hello!')"); // prints "hello!"
    return 0;
}
```

See [example](samples/abstraction/state/state.cpp).

## Use existing Lua state

```cpp
    lua_State *luaState = luaL_newstate();
    // luaState ownership is NOT transfered to luaStateView
    integral::StateView luaStateView(luaState); 
    luaStateView.openLibs();
    luaStateView.doString("print('hello!')"); // prints "hello!"
    lua_close(luaState);
```

See [example](samples/abstraction/state/state.cpp).

## Get and set value

```cpp
    luaState.doString("a = 42");
    int a = luaState["a"]; // "a" is set to 42
    luaState["b"] = "forty two";
    luaState.doString("print(b)"); // prints "forty two"

    luaState.doString("t = {'x', {pi = 3.14}}");
    std::cout << luaState["t"][2]["pi"].get<double>() << '\n'; // prints "3.14"
    luaState["t"]["key"] = "value";
    luaState.doString("print(t.key)"); // prints "value"
```

See [example](samples/abstraction/reference/reference.cpp).

## Register function

```cpp
double getSum(double x, double y) {
    return x + y;
}

double luaGetSum(lua_State *luaState) {
    integral::pushCopy(luaState, integral::get<double>(luaState, 1) + integral::get<double>(luaState, 2));
    return 1;
}

// ...

    luaState["getSum"].setFunction(getSum);
    luaState.doString("print(getSum(1, -.2))"); // prints "0.8"

    luaState["luaGetSum"].setLuaFunction(luaGetSum);
    luaState.doString("print(luaGetSum(1, -.2))"); // prints "0.8"

    // lambdas can be bound likewise
    luaState["printHello"].setFunction([]{
        std::puts("hello!");
    });
    luaState.doString("printHello()"); // prints "hello!"
```

See [example](samples/abstraction/function/function.cpp).

## Register function with default arguments

```cpp
    luaState["printArguments"].setFunction([](const std::string &string, int integer) {
        std::cout << string << ", " << integer << '\n';
    }, integral::DefaultArgument<std::string, 1>("default string"), integral::DefaultArgument<int, 2>(-1));
    luaState.doString("printArguments(\"defined string\")\n" // prints "defined string, -1"
                      "printArguments(nil, 42)\n" // prints "default string, 42"
                      "printArguments()"); // prints "default string, -1"
```
See [example](samples/abstraction/default_argument/default_argument.cpp).

## Register class

```cpp
class Object {
public:
    const std::string greeting_;
    std::string name_;

    Object(const std::string greeting, const std::string &name) : greeting_(greeting), name_(name) {}

    // const reference values are pushed by value (copied) to the Lua state
    const std::string & getGreeting() const {
        return greeting_;
    }

    std::string getHello() const {
        return greeting_ + ' ' + name_ + '!';
    }
};

// ...

        luaState["Object"] = integral::ClassMetatable<Object>()
                                 // invalid constructor register causes compilation error
                                 // .setConstructor<Object()>("invalid")
                                 .setConstructor<Object(const std::string &, const std::string &)>("new")
                                 .setFunction("getGreeting", &Object::getGreeting)
                                 .setGetter("getName", &Object::name_)
                                 .setSetter("setName", &Object::name_)
                                 .setFunction("getHello", &Object::getHello)
                                 .setFunction("getBye", [](const Object &object) {
                                    return std::string("Bye ") + object.name_ + '!';
                                 })
                                 .setLuaFunction("appendName", [](lua_State *lambdaLuaState) {
                                    // objects (except std::vector, std::array, std::unordered_map, std::tuple and std::string) are gotten by reference
                                    integral::get<Object>(lambdaLuaState, 1).name_ += integral::get<const char *>(lambdaLuaState, 2);
                                    return 1;
                                 });
```

See [example](samples/abstraction/class/class.cpp).

## Get object

Objects (except std::vector, std::array, std::unordered_map, std::tuple and std::string) are gotten by reference.

```cpp
    luaState.doString("object = Object.new('foo')\n"
                      "print(object:getName())"); // prints "foo"
    // objects (except std::vector, std::array, std::unordered_map, std::tuple and std::string) are gotten by reference
    luaState["object"].get<Object>().name_ = "foobar";
    luaState.doString("print(object:getName())"); // prints "foobar"
```

See [example](samples/abstraction/class/class.cpp).

## Register inheritance

```cpp
class BaseOfBase1 {
public:
    void baseOfBase1Method() const {
        std::puts("baseOfBase1Method");
    }
};

class Base1 : public BaseOfBase1 {
public:
    void base1Method() const {
        std::puts("base1Method");
    }
};

class Base2 {
public:
    void base2Method() const {
        std::puts("base2Method");
    }
};

class Derived : public Base1, public Base2 {
public:
    void derivedMethod() const {
        std::puts("derivedMethod");
    }
};

// ...

    integral::State luaState;
    luaState["BaseOfBase1"] = integral::ClassMetatable<BaseOfBase1>()
                              .setConstructor<BaseOfBase1()>("new")
                              .setFunction("baseOfBase1Method", &BaseOfBase1::baseOfBase1Method);
    luaState["Base1"] = integral::ClassMetatable<Base1>()
                        .setConstructor<Base1()>("new")
                        .setFunction("base1Method", &Base1::base1Method)
                        .setBaseClass<BaseOfBase1>();
    luaState["Base2"] = integral::ClassMetatable<Base2>()
                        .setConstructor<Base2()>("new")
                        .setFunction("base2Method", &Base2::base2Method);
    luaState["Derived"] = integral::ClassMetatable<Derived>()
                        .setConstructor<Derived()>("new")
                        .setFunction("derivedMethod", &Derived::derivedMethod)
                        .setBaseClass<Base1>()
                        .setBaseClass<Base2>();
    luaState.doString("derived = Derived.new()\n"
                      "derived:base1Method()\n" // prints "base1Method"
                      "derived:base2Method()\n" // prints "base2Method"
                      "derived:baseOfBase1Method()\n" // prints "baseOfBase1Method"
                      "derived:derivedMethod()"); // prints "derivedMethod"
```

See [example](samples/abstraction/inheritance/inheritance.cpp).

## Register table

```cpp
    luaState["group"] = integral::Table()
                            .set("constant", integral::Table()
                                .set("pi", 3.14))
                            .setFunction("printHello", []{
                                std::puts("Hello!");
                            })
                            .set("Object", integral::ClassMetatable<Object>()
                                .setConstructor<Object(const std::string &)>("new")
                                .setFunction("getHello", &Object::getHello));
    luaState.doString("print(group.constant.pi)\n" // prints "3.14"
                      "group.printHello()\n" // prints "Hello!"
                      "print(group.Object.new('object'):getHello())"); // prints "Hello object!"
```

See [example](samples/abstraction/table/table.cpp).

## Use polymorphism

Objects are automatically converted to base classes types regardless of inheritance definition with `integral`.

```cpp
class Base {};

class Derived : public Base {};

void callBase(const Base &) {
    std::puts("Base");
}

// ...

    luaState["Derived"] = integral::ClassMetatable<Derived>().setConstructor<Derived()>("new");
    luaState["callBase"].setFunction(callBase);
    luaState.doString("derived = Derived.new()\n"
                      "callBase(derived)"); // prints "Base"
```

See [example](samples/abstraction/polymorphism/polymorphism.cpp).

## Call function in Lua State

```cpp
    luaState.doString("function getSum(x, y) return x + y end");
    int x = luaState["getSum"].call<int>(2, 3); // 'x' is set to 5
```

See [example](samples/abstraction/function_call/function_call.cpp).

## Register lua function argument

```cpp
    luaState["getResult"].setFunction([](int x, int y, const integral::LuaFunctionArgument &function) {
        return function.call<int>(x, y);
    });
    luaState.doString("print(getResult(-1, 1, math.min))"); // prints "-1"
```

See [example](samples/abstraction/lua_function_argument/lua_function_argument.cpp).

## Table conversion

Lua tables are automatically converted to/from std::vector, std::array, std::unordered_map and std::tuple.

```cpp
    // std::vector
    luaState["intVector"] = std::vector<int>{1, 2, 3};
    luaState.doString("print(intVector[1] .. ' ' .. intVector[2] .. ' ' ..  intVector[3])"); // prints "1 2 3"

    // std::array
    luaState.doString("arrayOfVectors = {{'one', 'two'}, {'three'}}");
    std::array<std::vector<std::string>, 2> arrayOfVectors = luaState["arrayOfVectors"];
    std::cout << arrayOfVectors.at(0).at(0) << ' ' << arrayOfVectors.at(0).at(1) << ' ' << arrayOfVectors.at(1).at(0) << '\n'; // prints "one two three"

    // std::unordered_map and std::tuple
    luaState["mapOfTuples"] = std::unordered_map<std::string, std::tuple<int, double>>{{"one", {-1, -1.1}}, {"two", {1, 4.2}}};
    luaState.doString("print(mapOfTuples.one[1] .. ' ' .. mapOfTuples.one[2] .. ' ' .. mapOfTuples.two[1] .. ' ' .. mapOfTuples.two[2])"); // prints "-1 -1.1 1 4.2"
```

See [example](samples/abstraction/table_conversion/table_conversion.cpp)

## Register function with ignored argument

```cpp
// std::vector<double> is automatically converted to a lua table. Vector is a regular class for integral and is not converted to a lua table
class Vector : public std::vector<double> {};

// ...

        luaState["Vector"] = integral::ClassMetatable<Vector>()
                                .setConstructor<Vector()>("new")
                                // because of some legacy lua implementation details, __len receives two arguments, the second argument can be safely ignored
                                .setFunction("__len", [](const Vector &vector, integral::LuaIgnoredArgument) -> std::size_t {
                                    return vector.size();
                                })
                                .setFunction("pushBack", static_cast<void(Vector::*)(const double &)>(&Vector::push_back));

        luaState.doString("v = Vector.new()\n"
                          "print(#v)\n" // prints "0'
                          "v:pushBack(42)\n"
                          "print(#v)\n" // prints "1"
                          "v:pushBack(42)\n"
                          "print(#v)"); // prints "2"
```
See [example](samples/abstraction/ignored_argument/ignored_argument.cpp).

## Optional

std::optional<T> is automatically converted to/from nil/T.

```cpp
    luaState["g"].setFunction([](const std::optional<std::string> &optional) {
        if (optional.has_value() == false) {
            std::cout << "c++: hi!" << std::endl;
        } else {
            std::cout << "c++: hi " << optional.value() << '!' << std::endl;
        }
    });
    luaState.doString("g()"); // prints "c++: hi!"
    luaState.doString("g('world')"); // prints "c++: hi world!"
    std::optional<std::string> match;
    match = luaState["string"]["match"].call<std::optional<std::string>>("aei123bcd", "123");
    std::cout << match.value() << std::endl; // prints "123"
    match = luaState["string"]["match"].call<std::optional<std::string>>("aei123bcd", "xyz");
    std::cout << match.has_value() << std::endl; // prints "0"
```

See [example](samples/abstraction/optional/optional.cpp)

## Register synthetic inheritance

Synthetic inheritance can be viewed as a transformation from composition in c++ to inheritance in lua.

```cpp
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

    //...

        integral::State luaState;

        luaState["BaseOfSyntheticBase"] = integral::ClassMetatable<BaseOfSyntheticBase>()
                                          .setFunction("baseOfSyntheticBaseMethod", &BaseOfSyntheticBase::baseOfSyntheticBaseMethod);

        luaState["SyntheticBase"] = integral::ClassMetatable<SyntheticBase>()
                                    .setFunction("syntheticBaseMethod", &SyntheticBase::syntheticBaseMethod)
                                    .setBaseClass<BaseOfSyntheticBase>();

        luaState["SyntheticallyDerived"] = integral::ClassMetatable<SyntheticallyDerived>()
                                           .setConstructor<SyntheticallyDerived()>("new")
                                           .setFunction("syntheticallyDerivedMethod", &SyntheticallyDerived::syntheticallyDerivedMethod)
                                           .setBaseClass([](SyntheticallyDerived *syntheticallyDerived) -> SyntheticBase * {
                                               return &syntheticallyDerived->syntheticBase_;
                                           });

        luaState.doString("syntheticallyDerived = SyntheticallyDerived.new()\n"
                          "syntheticallyDerived:baseOfSyntheticBaseMethod()\n" // prints "baseOfSyntheticBaseMethod"
                          "syntheticallyDerived:syntheticBaseMethod()\n" // prints "syntheticBaseMethod"
                          "syntheticallyDerived:syntheticallyDerivedMethod()"); // prints "syntheticallyDerivedMethod"
```

See [example](samples/abstraction/synthetic_inheritance/synthetic_inheritance.cpp)

## std::reference_wrapper and std::shared_ptr automatic inheritance

`std::reference_wrapper<T>` and `std::shared_ptr<T>` are registered with automatic synthetic inheritance.

```cpp
    class Object {
    public:
        void printMessage() const {
            std::cout << "Object " << this << " message!" << std::endl;
        }
    };

    //...

        luaState["Object"] = integral::ClassMetatable<Object>()
            .setFunction("printMessage", &Object::printMessage);

        // std::reference_wrapper<T> has automatic synthetic inheritance do T as if it was defined as:
        // luaState.defineInheritance([](std::reference_wrapper<T> *referenceWrapperPointer) -> T * {
        //     return &referenceWrapperPointer->get();
        // });

        Object object;
        object.printMessage(); //prints 'Object 0x7ffee8b68560 message!'
        luaState["objectReference"] = std::ref(object);
        luaState.doString("objectReference:printMessage()"); //prints the same previous address 'Object 0x7ffee8b68560 message!'

        // std::shared_ptr<T> has automatic synthetic inheritance do T as if it was defined as:
        // luaState.defineInheritance([](std::shared_ptr<T> *sharedPtrPointer) -> T * {
        //     return sharedPtrPointer->get();
        // });

        std::shared_ptr<Object> objectSharedPtr = std::make_shared<Object>();
        objectSharedPtr->printMessage(); //prints 'Object 0x7fce594077a8 message!'
        luaState["objectSharedPtr"] = objectSharedPtr;
        luaState.doString("objectSharedPtr:printMessage()"); //prints the same previous address 'Object 0x7fce594077a8 message!'
        luaState["objectSharedPtr"].get<Object>().printMessage(); //prints the same previous address 'Object 0x7fce594077a8 message!'
```

See [example](samples/abstraction/reference_wrapper_and_shared_ptr/reference_wrapper_and_shared_ptr.cpp)


# Automatic conversion

`integral` does the following conversions:

| C++                                                              | Lua                                            |
| ---------------------------------------------------------------- | ---------------------------------------------- |
| integral types (`std::is_integral`)                              | number [integer subtype in Lua version >= 5.3] |
| floating point types (`std::is_floating_point`)                  | number [float subtype in Lua version >= 5.3]   |
| `bool`                                                           | boolean                                        |
| `std::string`, `const char *`                                    | string                                         |
| `std::vector`, `std::array`, `std::unordered_map`, `std::tuple`  | table                                          |
| `std::optional`                                                  | nil or converted value                         |
| from: `integral::LuaFunctionWrapper`, `integral::FunctionWrapper`| to: function                                   |
| to: `integral::LuaFunctionArgument`                              | from: function                                 |
| other class types                                                | userdata                                       |


# Automatic inheritance

`std::reference_wrapper<T>` and `std::shared_ptr<T>` have automatic synthetic inheritance to `T` (those types' lua objects inherit `T`'s lua methods and lua base classes).


# integral reserved names in Lua

`integral` uses the following names in Lua registry:

* `integral_LuaFunctionWrapperMetatableName`;
* `integral_TypeIndexMetatableName`;
* `integral_TypeManagerRegistryKey`; and
* `integral_InheritanceIndexMetamethodKey`.

The library also uses the following field names in its generated class metatables:

* `__index`;
* `__gc`;
* `integral_TypeFunctionsKey`;
* `integral_TypeIndexKey`;
* `integral_InheritanceKey`;
* `integral_AutomaticInheritanceKey`;
* `integral_UserDataWrapperBaseTableKey`;
* `integral_UnderlyingTypeFunctionKey`; and
* `integral_InheritanceSearchTagKey`.


# Source

`integral`'s Git repository is available on GitHub, which can be browsed at:

    http://github.com/aphenriques/integral

and cloned with:

    git clone --recurse-submodules git://github.com/aphenriques/integral.git


# Author

`integral` was made by André Pereira Henriques [aphenriques (at) outlook (dot) com].


# License

Copyright (C) 2013, 2014, 2015, 2016, 2017, 2019  André Pereira Henriques.

integral is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

integral is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

See file `COPYING` included with this distribution or check <http://www.gnu.org/licenses/> for license information.

![gplv3 logo](http://www.gnu.org/graphics/gplv3-127x51.png)

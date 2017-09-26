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
  * [Bind function](#bind-function)
  * [Bind class](#bind-class)
  * [Get object](#get-object)
  * [Bind table](#bind-table)
  * [Use polymorphism](#use-polymorphism)
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
* functions returning pointers (except const char * which is considered a string) and references are regarded as unsafe, therefore cannot be exported. Trying to register these functions will cause compilation error; and
* invalid default arguments definition causes compilation error.


# Requirements

* C++14 compiler; and
* Lua 5.1 or later.


# Tested environments

* gcc (Debian 4.9.2-10) 4.9.2 on Linux; and
* Apple LLVM version 7.0.2 (clang-700.1.81) on MacOS.


# Build

Grab all the source files (*.hpp and *.cpp) in `src` directory and build (there is no preprocessor configuration for the library).

Alternatively, build and install the library with:

    $ make
    $ make install


# Usage

Include the library header `integral.h` (`namespace integral`) and link to `libintegral.a` or `libintegral.so` if the library was built separately.

Check [samples/abstraction](samples/abstraction) directory for examples.

## Create Lua state

```cpp
#include "integral.h"

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

## Bind function

```cpp
double getSum(double x, double y) {
    return x + y;
}

double luaGetSum(lua_State *luaState) {
    integral::pushCopy(luaState, integral::get<double>(luaState, 1) + integral::get<double>(luaState, 2));
    return 1;
}

// ...

    integral::State luaState;
    luaState.openLibs();

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

## Bind class

```cpp
class Object {
public:
    std::string name_;

    Object(const std::string &name) : name_(name) {}

    std::string getHello() const {
        return std::string("Hello ") + name_ + '!';
    }
};

// ...

    luaState["Object"] = integral::ClassMetatable<Object>()
                             .setConstructor<Object(const std::string &)>("new")
                             .setCopyGetter("getName", &Object::name_)
                             .setSetter("setName", &Object::name_)
                             .setFunction("getHello", &Object::getHello)
                             .setFunction("getBye", [](const Object &object) {
                                return std::string("Bye ") + object.name_ + '!';
                             })
                             .setLuaFunction("appendName", [](lua_State *lambdaLuaState) {
                                // objects are gotten by reference
                                integral::get<Object>(lambdaLuaState, 1).name_ += integral::get<const char *>(lambdaLuaState, 2);
                                return 1;
                             });
```

See [example](samples/abstraction/class/class.cpp).

## Get object

Objects are gotten by reference.

```cpp
    luaState.doString("object = Object.new('foo')\n"
                      "print(object:getName())"); // prints "foo"
    // objects are gotten by reference
    luaState["object"].get<Object>().name_ = "foobar";
    luaState.doString("print(object:getName())"); // prints "foobar"
```

See [example](samples/abstraction/class/class.cpp).

## Bind table

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

## TODO


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
* `integral_UserDataWrapperBaseTableKey`;
* `integral_UnderlyingTypeFunctionKey`; and
* `integral_InheritanceSearchTagKey`.


# Source

`integral`'s Git repository is available on GitHub, which can be browsed at:

    http://github.com/aphenriques/integral

and cloned with:

    git clone git://github.com/aphenriques/integral.git


# Author

`integral` was made by André Pereira Henriques [aphenriques (at) outlook (dot) com].


# License

Copyright (C) 2013, 2014, 2015, 2016, 2017  André Pereira Henriques.

integral is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

integral is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

See file `COPYING` included with this distribution or check <http://www.gnu.org/licenses/> for license information.

![gplv3 logo](http://www.gnu.org/graphics/gplv3-127x51.png)

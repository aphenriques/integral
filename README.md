# integral

`integral` is a C++ library for binding C++ code with lua. It utilizes C++ template meta programming to interface both languages.

![Lua logo](http://www.lua.org/images/lua-logo.gif)


# Source

`integral`'s Git repository is available on GitHub, which can be browsed at:

    http://github.com/aphenriques/integral

and cloned with:

    git clone git://github.com/aphenriques/integral.git


# Features

## Concise API

* no macros;
* it follows the stack based structure of Lua API. Both APIs are fully compatible and can be mixed;
* there is no manual initialization of the library. Each of its functions is autonomous.

## Dependency-free build

* no external library is required in order to build it.

## Small intrusion on the lua state

* `integral` has a few reserved names (see **integral reserved names in Lua** ahead);
* the library allows integration with other bound libraries (wether using `integral` - statically *or* dynamically linked - or not);
* the library is thread safe (as per lua state).

## Automatic type management

* bound C/C++ types are not needed to be given a name;
* types are automatically registered;
* inheritance support;
* automatic type conversion to base types;
* custom type conversions.

## Memory safety

* every userdata object and function object is provided a garbage-collecttion metamethod;
* integral does stack unwinding before the Lua error handling gets in action.

## Error safety

* `integral` will not crash the lua state;
* thrown exceptions in exported functions are converted to lua errors;
* wrong parameter types in exported functions turn into lua errors;
* wrong number of parameters in exported functions turn into lua errors;
* functions returning pointers (except const char * - string) and references are regarded as unsafe, therefore cannot be exported. Trying to register these functions will cause compilation error;
* invalid default arguments definition causes compilation error.


# Example

```cpp
#include <iostream>
#include <string>
#include <lua.hpp>
#include "integral.h"

class Object {
public:
    Object(const char * name) : name_(name) {}

    void printMessage(const std::string &message) const {
        std::cout << "Message of Object '" << name_ << "': " << message << std::endl;
    }

private:
    const std::string name_;
};

int main(int argc, char * argv[]) {
    lua_State *luaState = luaL_newstate();
    integral::pushClassMetatable<Object>(luaState);
    integral::setConstructor<Object, const char *>(luaState, "new");
    integral::setFunction(luaState, "print", &Object::printMessage);
    // setting functions and constructors does not change the stack (like Lua API)
    lua_setglobal(luaState, "Object");
    luaL_dostring(luaState, "local object = Object.new(\"MEPHI\")\n"
                            "object:print(\"destroy the Green Wall!\")");
    // prints: Message of Object 'MEPHI': destroy the Green Wall!
    lua_close(luaState);
    return 0;
}
```


# Install

`integral` builds on Linux and MacOSX (Windows has not been tested yet). It requires a C++11 compatible compiler. The library has been successfully built with:

* Apple LLVM version 5.0; and
* gcc (Debian 4.7.2-5) 4.7.2

Modify `common.mk` for Lua and `integral` install configuration.

Any of the following make invocations can be executed with the `-j` option (`make -j`) for parallel compilation (builds faster).

To build the library:

    $ make

The shared and static library files will be on `src/` directory. The include files needed to use `integral` are on `src/` directory.

To install:

    $ sudo make install

To uninstall:

    $ sudo make uninstall

To build the samples:

    $ make samples

To clean everything compiled (including samples):

    $ make clean


# Usage

Set the compiler standard to C++11, include `integral` headers in the search path and link to its library. E.g:

    compiler flag: `-std=c++11 -I/usr/local/integral`
    linker flags: `-L/usr/local/lib -lintegral`

Include the library header `integral.h` (`namespace integral`). This header includes `core.h` and `DefaultArgument.h`.

The library interface is comprised of the functions in `core.h`, which has a brief description for each of them.

Check the `samples` directory for examples.


# Samples

First, build the samples from `integral` root directory with:

    $ make samples

For all the samples, except `embedded`, run the lua script in the directory:

    $ lua <name of the sample>.lua

For `embedded`, run the compiled executable `embedded`

    $ ./embedded


# integral reserved names in Lua

`integral` uses the following names in Lua registry:

* `integral_LuaFunctionWrapperMetatableName`;
* `integral_TypeIndexMetatableName`;
* `integral_TypeManagerRegistryKey`;
* `integral_InheritanceIndexMetatable`.

The library also makes use of the following field names for its generated class metatables:

* `__index`;
* `__gc`;
* `integral_TypeFunctionsKey`;
* `integral_TypeIndexKey`;
* `integral_InheritanceKey`;
* `integral_UserDataWrapperBaseTableKey`;
* `integral_UnderlyingTypeFunctionKey`;
* `integral_InheritanceSearchTagKey`.

# Author

`integral` was made by André Pereira Henriques [aphenriques (at) outlook (dot) com].


# License

Copyright (C) 2013, 2014  André Pereira Henriques.

integral is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

integral is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

See file `COPYING` included with this distribution or check <http://www.gnu.org/licenses/> for license information.

![gplv3 logo](http://www.gnu.org/graphics/gplv3-127x51.png)

# integral

`integral` is a C++ library for generating lua bindings. It is **not** a binding-code generator. It utilizes template meta programming to expose C/C++ functions and classes to lua, directly from C++ code.


# Source

`integral`'s Git repository is available on GitHub, which can be browsed at:

    http://github.com/aphenriques/integral

and cloned with:

    git clone git://github.com/aphenriques/integral.git


# Features

## Concise API

* it is comprised of few but flexible functions;
* it follows the stack based structure of Lua API. Both APIs are fully compatible and can be mixed;
* there is no manual initialization of the library. Each of its functions is autonomous.

## Dependency-free build

* no external library is required in order to build it.

## Small intrusion on the lua state

* it does not polute the lua state environment, allowing integration with other bound libraries (using `integral` or not);
* the library is thread safe (as per lua state).

## Automatic type management

* bound C/C++ types are not needed to be given a name;
* inheritance support.

## Memory safety

* every userdata object and function object is provided a garbage-collecttion metamethod;
* integral does stack unwinding before the Lua error handling gets in action.

## Error safety

* `integral` will not crash the lua state;
* thrown exceptions are converted to lua errors;
* wrong parameter types turn into lua errors;
* functions returning pointers (except char * - string) and references are regarded as unsafe, therefore not supported. Trying to register these functions will cause compilation error. Storing a reference to an object or to a value that has already been garbage collected would lead to undefined behaviour.


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
    integral::core::pushClassMetatable<Object>(luaState);
    integral::core::setConstructor<Object, const char *>(luaState, "new");
    integral::core::setFunction(luaState, "print", &Object::printMessage);
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

`integral` builds on Linux and MacOSX (Windows has not been tested yet). It requires a modern C++ compiler. The library has been successfully built with:

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

Include integral headers in your project search path and link to `integral` library. E.g:

    compiler flag: `-I/usr/local/integral`
    linker flags: `-L/usr/local/lib -lintegral`

The library is comprised of the functions in `core.h` (`namespace integral::core`), which has a brief description for each of them.

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
* `integral_TypeManagerRegistryKey`.

The library also makes use of the following field names for its generated class metatables:

* `__gc`;
* `integral_TypeFunctionsKey`;
* `integral_TypeIndexKey`.


# Author

`integral` was made by André Pereira Henriques [aphenriques (at) outlook (dot) com].


# License

Copyright (C) 2013  André Pereira Henriques.

integral is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

integral is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

See file `COPYING` included with this distribution for license information or check <http://www.gnu.org/licenses/>.

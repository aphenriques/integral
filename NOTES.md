## TODO
* remove underscores from filenames and namespaces (make everything camel case)
* update README.md
  * usage
    * LuaIgnoredArgument
    * module
    * ...
  * highlight comprehensive error messages 
* intel compiler
* coverity
* c++17:
  * string_view
  * type_traits is*_v and *_v
  * enable -Wunused-parameter and use [[maybe_unused]]
  * fold expressions (search and replace expansion/recursion functions)
  * template type deduction by constructor
  * makeFunctionWrapper and makeConstructorWrapper are not necessary in c++17 (template type deduction in constructor)
* compare performance with other binding libraries
* sandbox (in lua script?)
  * must be easily disabled
  * hide:
    * registry reserved names
    * class metatable reserved names
  * http://lua-users.org/wiki/SandBoxes
* frequent
  * run test
  * build all samples
  * check inlines
  * check includes (alphabetical order; necessity)
  * check memory leakage
  * check copyright notices
  * check compatibility with lua 5.1, 5.2 and 5.3 and luaJIT
  * update compatibility information on README.md and lua wiki

## Rationales
* removing occurrences of exchanger::Exchanger outside of exchanger.h is not possible because LuaFunctionArgument.h includes Caller.h which includes exchanger.h (cyclic dependency due to LuaFunctionArgument::call)
* thought overload functions implementation methods result in high performance overhead and inconsistent behaviour with c++ overload mechanism
* LuaPack (multiple return) adds complexity spread out through too much code (high maintence). Adaptors such as LuaTuple offers similar functionality
* safety, robustness and clarity must be prioritized over performance:
  * shrinking integral reserved names string size decreases clarity (and it had little performance impact on preliminary tests)
  * script integration should not be used for logic intensive operations
  * usage
    * monitoring
    * configuration
    * adjustments
    * prototyping

## Problems
* Xcode migh emmit a linker warning in the form:
  * "Direct access in function ..."
  * To fix this, on target (not project) settings set:
    * Inline Methods Hidden > No

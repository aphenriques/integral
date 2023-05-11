## TODO
* intel compiler
* coverity
* compare performance with other binding libraries
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
* placement new is done on adjusted userdata allocated space to avoid constructor call on misaligned address
  * see basic::pushAlignedObject
* removing occurrences of exchanger::Exchanger outside of exchanger.h is not possible because LuaFunctionArgument.h includes Caller.h which includes exchanger.h (cyclic dependency due to LuaFunctionArgument::call)
* thought overload functions implementation methods result in high performance overhead and inconsistent behaviour with c++ overload mechanism
* LuaPack (multiple return) adds complexity spread out through too much code (high maintence). Adaptors such as LuaTuple offers similar functionality
* safety, robustness and clarity must be prioritized over performance:
  * shrinking integral reserved names string size decreases clarity (and it had little performance impact on preliminary tests)
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
* error using -flto compiler flag: "ar: <file>.o: plugin needed to handle lto object"
  * define:
    * AR=gcc-ar

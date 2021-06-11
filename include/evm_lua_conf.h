#ifndef EVM_LUA_CONF_H
#define EVM_LUA_CONF_H

#define EVM_LUA_HEAP_SIZE    (5 * 1024)
#define EVM_LUA_STACK_SIZE   (2 * 1024)
#define EVM_LUA_SIZE_OF_GLOBALS     20

/*
@@ LUA_IDSIZE gives the maximum size for the description of the source
@@ of a function in debug information.
** CHANGE it if you want a different size.
*/
#define LUA_IDSIZE	60

#endif

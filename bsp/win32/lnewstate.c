#include "lua.h"
#include "ecma.h"

static char _path[256];
const char * vm_load(evm_t * e, char * path, int type)
{
    char* buffer = NULL;
    FILE *file;
    uint32_t lSize;

    sprintf(_path, "../../test/elua/%s", path);
    sprintf(e->file_name, "%s", _path);

    file = fopen(_path, "rb");
    if (file == NULL)
        return NULL;
    fseek (file , 0 , SEEK_END);
    lSize = (uint32_t)ftell (file);
    rewind (file);
    evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*lSize + 1);
    buffer = (char*)evm_buffer_addr(b);
    memset(buffer, 0, lSize + 1);
    fread(buffer, 1, lSize, file);
    buffer[lSize] = 0;
    fclose(file);
    return buffer;
}


static evm_t * env = NULL;

LUA_API lua_State *lua_newstate (lua_Alloc f, void *args) {
    evm_lua_set_allocf(f, args);
    evm_register_print(printf);
    evm_register_file_load((intptr_t)vm_load);
    env = (evm_t*)evm_malloc(sizeof(evm_t));
    if( env == NULL )
      return NULL;
    memset(env, 0, sizeof(evm_t));
    evm_err_t err = evm_init(env, EVM_LUA_HEAP_SIZE, EVM_LUA_STACK_SIZE, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);
    if( err != ec_ok )
      return NULL;

    err = ecma_module(env);
    if (err != ec_ok)
        return NULL;

    return evm_lua_new_state(env, EVM_LUA_SIZE_OF_GLOBALS);
}



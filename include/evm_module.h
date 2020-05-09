#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"
#include "nevm.h"
#include "evm_board.h"

#define EVM_ARG_LENGTH_ERR {evm_set_err(e, ec_type, "Bad argument length"); return EVM_VAL_UNDEFINED;}
#define EVM_ARG_TYPE_ERR {evm_set_err(e, ec_type, "Bad argument type"); return EVM_VAL_UNDEFINED;}

int evm_module(evm_t * e);
void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, const char * api_name);
int evm_module_add_callback(evm_val_t callback, evm_val_t args);
int evm_module_remove_callback(int id);
evm_val_t * evm_module_get_callback(int id);
extern evm_t * evm_runtime;

#endif

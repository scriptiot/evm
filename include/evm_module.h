#ifndef EVM_MODULE_H
#define EVM_MODULE_H

#include "evm.h"
#include "nevm.h"

#define EVM_ARG_LENGTH_ERR {evm_set_err(e, ec_type, "Bad argument length"); return EVM_VAL_UNDEFINED;}
#define EVM_ARG_TYPE_ERR {evm_set_err(e, ec_type, "Bad argument type"); return EVM_VAL_UNDEFINED;}

int evm_module(evm_t * e);

void evm_module_construct(evm_t* e, evm_val_t * p, int argc, evm_val_t * v, const char * api_name);

#endif


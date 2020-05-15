#ifndef ECMA_H
#define ECMA_H

#include "evm.h"

#ifdef CONFIG_EVM_ECMA_OBJECT
#define ECMA_OBJECT_ATTR_SIZE   6
#define ECMA_OBJECT_PROP_SIZE   9
extern evm_val_t * ecma_Object;
#endif

#ifdef CONFIG_EVM_ECMA_ARRAY
#define ECMA_ARRAY_ATTR_SIZE    6
extern evm_val_t * ecma_Array;
#endif

#ifdef CONFIG_EVM_ECMA_REGEXP
extern evm_val_t * ecma_RegExp;
#endif

#ifdef CONFIG_EVM_ECMA_MATH
extern evm_val_t * ecma_Math;
#endif

#ifdef CONFIG_EVM_ECMA_NUMBER
extern evm_val_t * ecma_Number;
#define ECMA_NUMBER_ATTR_SIZE   3
#define ECMA_NUMBER_PROP_SIZE   2
#endif

#ifdef CONFIG_EVM_ECMA_STRING
extern evm_val_t * ecma_String;
#define ECMA_STRING_ATTR_SIZE   3
#define ECMA_STRING_PROP_SIZE   2
#endif

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");return EVM_VAL_UNDEFINED; }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");return EVM_VAL_UNDEFINED; }

extern evm_val_t * ecma_function_prototype;
extern evm_val_t * ecma_object_prototype;

int ecma_module(evm_t * e);

#endif

#ifndef _PYTHON_BUILTINS_H
#define _PYTHON_BUILTINS_H

#include "evm.h"

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "invalid argument length");return evm_mk_undefined(); }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "invalid argument type");return evm_mk_undefined(); }

#endif

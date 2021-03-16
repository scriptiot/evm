/****************************************************************************
**
** Copyright (C) 2021 @武汉市字节码科技有限公司
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持语言类型：JavaScript, Python, QML, EVUE, JSON, XML, HTML, CSS
**  Version	: 3.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
****************************************************************************/

#ifndef ECMA_H
#define ECMA_H

#include "evm.h"

#ifdef CONFIG_EVM_ECMA_PROPERTY_FLAG
extern evm_hash_t ecma_hash_value;
extern evm_hash_t ecma_hash_writable;
extern evm_hash_t ecma_hash_enumerable;
extern evm_hash_t ecma_hash_configurable;
extern evm_hash_t ecma_hash_property_flag;
#endif

#define ECMA_OP_HASH_ARRAY  EVM_INVALID_HASH

typedef struct ecma_builtin_t {
    const char *name;
    evm_val_t (*func)(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
} ecma_builtin_t;

#define CONFIG_EVM_ECMA_DEFAULT_MEMBER_LEN  8

#ifdef CONFIG_EVM_ECMA_OBJECT
extern evm_val_t * ecma_Object;
#endif

#ifdef CONFIG_EVM_ECMA_ARRAY
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
#endif

#ifdef CONFIG_EVM_ECMA_STRING
extern evm_val_t * ecma_String;
#endif

#ifdef CONFIG_EVM_ECMA_BOOLEAN
extern evm_val_t * ecma_Boolean;
extern evm_val_t *ecma_boolean_init(evm_t *e);
#endif

#ifdef CONFIG_EVM_ECMA_JSON
extern evm_val_t *ecma_JSON;
extern evm_val_t *ecma_json_init(evm_t * e);
#endif

#ifdef CONFIG_EVM_ECMA_CONSOLE
extern evm_val_t * ecma_console;
extern evm_val_t * ecma_console_init(evm_t *e);
#endif

#ifdef CONFIG_EVM_ECMA_TIMEOUT
#define CONFIG_EVM_ECMA_TIMEOUT_SIZE   16
void ecma_timeout_poll(evm_t *e);
#endif

#define ARG_LENGTH_ERR { evm_set_err(e, ec_type, "Invalid argument length");return EVM_VAL_UNDEFINED; }
#define ARG_TYPE_ERR   { evm_set_err(e, ec_type, "Invalid argument type");return EVM_VAL_UNDEFINED; }

evm_err_t ecma_module(evm_t * e);

extern evm_val_t * ecma_function_prototype;
extern evm_val_t * ecma_object_prototype;
extern evm_val_t * ecma_array_prototype;

extern evm_object_native_t ecma_native;

extern evm_hash_t ECMA_HASH_PRIMITIVE;
extern evm_hash_t ECMA_HASH_SETTER;
extern evm_hash_t ECMA_HASH_GETTER;
extern evm_hash_t ecma_hash_constructor;
extern evm_hash_t ecma_hash_prototype;
extern evm_hash_t ecma_hash___proto__;
extern evm_hash_t ecma_hash_toString;
extern evm_hash_t ecma_hash_length;

extern evm_object_native_t _ecma_object_native;

EVM_API evm_val_t ecma_builtin_get_prop(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t *v, evm_err_t *ok);
EVM_API evm_val_t ecma_builtin_set_prop(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t *v, evm_err_t *ok);
EVM_API evm_val_t *ecma_builtin_get_array(evm_t * e, evm_val_t * p, int argc, evm_val_t *v);
EVM_API evm_val_t ecma_builtin_iterator(evm_t * e, evm_val_t * p, int argc, evm_val_t *v);
EVM_API evm_val_t ecma_builtin_iterator_init(evm_t * e, evm_val_t * p, int argc, evm_val_t *v);

EVM_API evm_val_t ecma_builtin_call(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
EVM_API evm_val_t ecma_builtin_bind(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
EVM_API evm_val_t ecma_builtin_apply(evm_t *e, evm_val_t *p, int argc, evm_val_t *v);
EVM_API evm_val_t ecma_builtin_hasOwnProperty(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
EVM_API evm_val_t ecma_builtin_isPrototypeOf(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
EVM_API evm_val_t ecma_builtin_valueOf(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

EVM_API evm_err_t ecma_create_attributes(evm_t *e, evm_val_t *obj);
EVM_API evm_val_t *ecma_builtin_create_function(evm_t *e, evm_object_native_t *native);

EVM_API evm_val_t ecma_array_prototype_constructor(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
EVM_API evm_val_t ecma_array_prototype_toString(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

EVM_API void ecma_object_attrs_apply(evm_t * e, evm_val_t * o, evm_val_t * prototype);
EVM_API evm_val_t ecma_object_prototype_toString(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

EVM_API evm_val_t ecma_array_constructor(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

EVM_API void ecma_array_prototype_create(evm_t * e);
EVM_API evm_err_t ecma_array_prototype_attr_create(evm_t *e, evm_val_t *o);
EVM_API void ecma_function_prototype_create(evm_t * e);
EVM_API void ecma_object_prototype_create(evm_t * e);
EVM_API evm_val_t ecma_object_get_val_from_proto(evm_t * e, evm_val_t * o, evm_hash_t name);
EVM_API evm_err_t ecma_builtin_property_flag_create(evm_t *e, evm_val_t *o);

#endif

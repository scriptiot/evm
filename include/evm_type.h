#ifndef EVM_TYPE_H
#define EVM_TYPE_H

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

typedef uint64_t evm_val_t;
typedef uint32_t evm_err_t;

typedef intptr_t evm_hash_t;
typedef uint32_t evm_type_t;
typedef int32_t  evm_int_t;

/**
 * EVM虚拟机语言种类支持
 */
#define EVM_LANG_PY     1   /** python语法*/
#define EVM_LANG_JS     2   /** JavaScript语法*/
#define EVM_LANG_LUA    3   /** LUA语法*/
#define EVM_LANG_JSON   4   /** JSON语法*/
#define EVM_LANG_XML    5   /** XML语法*/
#define EVM_LANG_QML    6   /** QML语法*/
#define EVM_LANG_HML    7   /** HML语法*/
#define EVM_LANG_CSS    8   /** CSS语法*/
#define EVM_LANG_HTML   9   /** HTML语法*/

#define EVM_VER                   300  /** 虚拟机版本号*/
#define EVM_VAR_NAME_MAX_LEN      255  /** 虚拟机解析文件变量名称最大长度*/
#define EVM_FILE_NAME_LEN         255  /** 文件名称长度*/
#define EVM_LOAD_MODULE      0      /** 虚拟机加载模块*/
#define EVM_LOAD_MAIN        1      /** 虚拟机加载main*/
#define EVM_UNUSED(x) ((void) (x))
#define EVM_GC_TYPE_MASK(v)  (v & 0xff)
#define EVM_HEAP_BLOCK_SIZE  32
#define EVM_DO_NOT_REFRESH_MODULE   0
#define EVM_REFRESH_MODULE          1
#define EVM_INVALID_HASH           -1

#define EVM_RUN_AS_MODULE           0
#define EVM_RUN_WITH_RETURN         1


enum GC_TYPE
{
    GC_NONE,
    GC_TOKEN,
    GC_ROOT,
    GC_OBJECT,
    GC_NATIVE_OBJECT,
    GC_DICT,
    GC_CLASS,
    GC_SET,
    GC_FUNC,
    GC_CLOSURE_FUNC,
    GC_STATIC_FUNC,
    GC_LIST,
    GC_BUFFER,
    GC_BUFFER16,
    GC_BUFFER32,
    GC_BUFFER64,
    GC_BUFFER_FLOAT,
    GC_BUFFER_DOUBLE,
    GC_BUFFER_OBJECT,
    GC_STRING,
    GC_NUMBER,
    GC_BOOLEAN,
    GC_TUPLE,
    GC_MEMBER,
    GC_MEMBER_KEYS,
    GC_MEMBER_VALS,
};

enum EVM_TYPE
{
    TYPE_NUMBER = 0,
    TYPE_HEAP_STRING,
    TYPE_FOREIGN_STRING,
    TYPE_BOOLEAN,
    TYPE_FUNCTION,
    TYPE_NATIVE,
    TYPE_NULL,
    TYPE_UNDEFINED,
    TYPE_NAN,
    TYPE_LIST,
    TYPE_BUFFER,
    TYPE_OBJECT,
    TYPE_CLASS,
    TYPE_FRAME,
    TYPE_FOREIGN,
    TYPE_INVOKE,
};

enum Errcode
{
    ec_ok = 0,
    ec_err,
    ec_no_file,
    ec_file_name_len,
    ec_name,
    ec_type,
    ec_memory,
    ec_zero_division,
    ec_syntax,
    ec_index,
    ec_import,
    ec_attribute,
    ec_key,
    ec_value,
    ec_system,
    ec_assertion,
    ec_overflow,
    ec_arithmetic,
    ec_indent,
    ec_gc,
    ec_exit,
};

typedef evm_val_t (*evm_native_fn)(void * e, evm_val_t * self, int vc, evm_val_t * v);

typedef struct evm_builtin_t{
    const char * name;
    evm_val_t v;
}evm_builtin_t;

typedef struct heap_t {
    uint32_t block_size;
    uint32_t * map;
    uint32_t map_size;
    uint32_t size;
    uint32_t free;
    uint32_t offset;
    uint32_t usage;
    void *base;
} heap_t;

typedef struct evm_const_pool_t{
    int count;
    evm_val_t * values;
    struct evm_const_pool_t * next;
}evm_const_pool_t;

typedef struct evm_t{
    evm_err_t err;
    uint32_t line;
    const char * err_arg;
    char *file_name;
    uint32_t file_name_len;
    char *var_name;
    uint32_t var_name_len;

    heap_t *heap;
    evm_val_t *sp;
    evm_val_t *sp_base;
    uint32_t stack_size;
    evm_val_t root;
    evm_val_t scope;

    uint32_t native_cnt;
    evm_hash_t *native_symbals;
    evm_val_t *native_tbl;

    evm_val_t *module_tbl;

    evm_const_pool_t * string_pool;
    evm_const_pool_t * number_pool;

    evm_val_t *builtin_number;
    evm_val_t *builtin_string;
    evm_val_t *builtin_boolean;

    uint32_t gc_usage;
}evm_t;

typedef void (*evm_init_fn)(evm_t * e, evm_val_t * old_self, evm_val_t * new_self);
typedef void (*evm_destroy_fn)(evm_t * e, evm_val_t *self);

typedef union {
    evm_val_t   v;
    double      d;
} evm_valnum_t;

typedef struct evm_object_native_t {
    evm_val_t (*creator)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
    evm_val_t (*destroyer)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
    evm_val_t (*setter)(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok);
    evm_val_t (*getter)(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok);
    evm_val_t (*caller)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

    evm_val_t (*get_prop)(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok);
    evm_val_t (*set_prop)(evm_t * e, evm_val_t * p, evm_hash_t name, int argc, evm_val_t * v, evm_err_t *ok);
    evm_val_t *(*get_array)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
    evm_val_t (*iterator)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);
    evm_val_t (*iterator_init)(evm_t * e, evm_val_t * p, int argc, evm_val_t * v);

    evm_err_t (*operate)(evm_t * e, evm_val_t *op1, evm_val_t *op2, evm_val_t *res);
    void *user_data;
} evm_object_native_t;

#define EVM_API     extern
#define double_2_val(d) (((evm_valnum_t*)&(d))->v)
#define MAKE_TYPE(s, t) \
  ((evm_val_t)(s) << 63 | (evm_val_t) 0x7ff0 <<48 | (evm_val_t)(t) <<48)
#define INNER_TYPE_INFINITE        MAKE_TYPE(0, TYPE_NUMBER)
#define INNER_TYPE_FRAME           MAKE_TYPE(1, TYPE_FRAME)
#define INNER_TYPE_BUFFER          MAKE_TYPE(1, TYPE_BUFFER)
#define INNER_TYPE_STRING_H        MAKE_TYPE(1, TYPE_HEAP_STRING)
#define INNER_TYPE_STRING_F        MAKE_TYPE(1, TYPE_FOREIGN_STRING)
#define INNER_TYPE_BOOLEAN         MAKE_TYPE(1, TYPE_BOOLEAN)
#define INNER_TYPE_FUNC_SCRIPT     MAKE_TYPE(1, TYPE_FUNCTION)
#define INNER_TYPE_FUNC_NATIVE     MAKE_TYPE(1, TYPE_NATIVE)
#define INNER_TYPE_NAN             MAKE_TYPE(1, TYPE_NAN)
#define INNER_TYPE_LIST            MAKE_TYPE(1, TYPE_LIST)
#define INNER_TYPE_FOREIGN         MAKE_TYPE(1, TYPE_FOREIGN)
#define INNER_TYPE_UNDEFINED       MAKE_TYPE(1, TYPE_UNDEFINED)
#define INNER_TYPE_NULL            MAKE_TYPE(1, TYPE_NULL)
#define INNER_TYPE_OBJECT          MAKE_TYPE(1, TYPE_OBJECT)
#define INNER_TYPE_CLASS           MAKE_TYPE(1, TYPE_CLASS)
#define INNER_TYPE_INVOKE          MAKE_TYPE(0, TYPE_INVOKE)
#define INNER_TYPE_MASK            MAKE_TYPE(1, 0xF)
#define EVM_VAL_MASK               (~MAKE_TYPE(1, 0xF))
#define EVM_INT_TYPE_MASK          0x0F00000000000000
#define EVM_ADDR_MASK              0x0000FFFFFFFFFFFF
#define EVM_VAL_UNDEFINED       INNER_TYPE_UNDEFINED
#define EVM_VAL_NULL            INNER_TYPE_NULL
#define EVM_VAL_INFINITE        INNER_TYPE_INFINITE
#define EVM_VAL_TRUE            (INNER_TYPE_BOOLEAN | 1)
#define EVM_VAL_FALSE           (INNER_TYPE_BOOLEAN)

static inline int evm_type(evm_val_t *v) {
    int type = (*v) >> 48;
    if ((type & 0x7ff0) != 0x7ff0) {
        return 0;
    } else {
        return type & 0xf;
    }
}

static inline double evm_2_double(evm_val_t *v) {
    return ((evm_valnum_t*)v)->d;
}

static inline evm_int_t evm_2_integer(evm_val_t *v) {
    return (evm_int_t)((evm_valnum_t*)v)->d;
}

static inline intptr_t evm_2_intptr(evm_val_t *v) {
    return (intptr_t)(*v & EVM_VAL_MASK);
}

static inline void *evm_2_object(evm_val_t *v) {
    return (void*)(*v & EVM_VAL_MASK);
}

static inline int evm_2_boolean(evm_val_t *v) {
    return evm_2_intptr(v);
}

static inline int evm_is_number(evm_val_t *v) {
    return (*v & INNER_TYPE_INFINITE) != INNER_TYPE_INFINITE;
}

static inline int evm_is_integer(evm_val_t *v){
    if(!evm_is_number(v)) return 0;
    if(evm_2_double(v) - evm_2_integer(v) == 0) return 1;
    return 0;
}

static inline int evm_is_heap_string(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_STRING_H;
}

static inline int evm_is_foreign_string(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_STRING_F;
}

static inline int evm_is_string(evm_val_t *v) {
    return evm_is_heap_string(v) || evm_is_foreign_string(v);
}

static inline int evm_is_boolean(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_BOOLEAN;
}

static inline int evm_is_buffer(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_BUFFER;
}

static inline int evm_is_script(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FUNC_SCRIPT;
}

static inline int evm_is_native(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FUNC_NATIVE;
}

static inline int evm_is_list(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_LIST;
}

static inline int evm_is_foreign(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FOREIGN;
}

static inline int evm_is_function(evm_val_t *v) {
    return evm_is_script(v) || evm_is_native(v);
}

static inline int evm_is_undefined(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_UNDEFINED;
}

static inline int evm_is_null(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_NULL;
}

static inline int evm_is_nan(evm_val_t *v)
{
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_NAN;
}

static inline int evm_is_object(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_OBJECT;
}

static inline int evm_is_class(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_CLASS;
}

static inline int evm_is_true(evm_val_t *v) {
    if( evm_is_boolean(v) && (evm_2_boolean(v) == 1 ) )
        return 1;
    return 0;
}

static inline int evm_is_false(evm_val_t *v) {
    if( evm_is_boolean(v) && (evm_2_boolean(v) == 0 ) )
        return 1;
    return 0;
}

const char *evm_2_string(evm_val_t *v);

static inline evm_val_t evm_mk_number(double d) {
    return double_2_val(d);
}

static inline evm_val_t evm_mk_foreign_string(const char *s) {
    return INNER_TYPE_STRING_F | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_heap_string(intptr_t v) {
    return INNER_TYPE_STRING_H | (v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_boolean(int v) {
    return INNER_TYPE_BOOLEAN | (!!v);
}

static inline evm_val_t evm_mk_script(intptr_t v) {
    return INNER_TYPE_FUNC_SCRIPT | (v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_native(intptr_t v) {
    return INNER_TYPE_FUNC_NATIVE | (v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_null(void) {
    return INNER_TYPE_NULL;
}

static inline evm_val_t evm_mk_undefined(void) {
    return INNER_TYPE_UNDEFINED;
}

static inline evm_val_t evm_mk_object(void *v) {
    return INNER_TYPE_OBJECT | ((intptr_t)v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_buffer(void *v) {
    return INNER_TYPE_BUFFER | ((intptr_t)v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_list(void *v) {
    return INNER_TYPE_LIST | ((intptr_t)v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_foreign(void *v) {
    return INNER_TYPE_FOREIGN | ((intptr_t)v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_class(void *v) {
    return INNER_TYPE_CLASS | ((intptr_t)v & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_true(void) {
    return EVM_VAL_TRUE;
}

static inline evm_val_t evm_mk_false(void) {
    return EVM_VAL_FALSE;
}

static inline evm_val_t evm_mk_nan(void)
{
    return INNER_TYPE_NAN;
}

static inline void evm_set_boolean(evm_val_t *v, int b) {
    *((uint64_t *)v) = INNER_TYPE_BOOLEAN | !!b;
}

static inline void evm_set_number(evm_val_t *v, double d) {
    *((double *)v) = d;
}

static inline void evm_set_nan(evm_val_t *v)
{
    *((uint64_t *)v) = INNER_TYPE_NAN;
}

static inline void evm_set_foreign_string(evm_val_t *v, const char *s) {
    *((uint64_t *)v) = INNER_TYPE_STRING_F | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline void evm_set_heap_string(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_STRING_H | (s & EVM_ADDR_MASK);
}

static inline void evm_set_script(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_FUNC_SCRIPT | (s & EVM_ADDR_MASK);
}

static inline void evm_set_native(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_FUNC_NATIVE | (s & EVM_ADDR_MASK);
}

static inline void evm_set_undefined(evm_val_t *v) {
    *((uint64_t *)v) = INNER_TYPE_UNDEFINED;
}

static inline void evm_set_null(evm_val_t *v) {
    *((uint64_t *)v) = INNER_TYPE_NULL;
}

static inline void evm_set_object(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_OBJECT | (s & EVM_ADDR_MASK);
}

static inline void evm_set_class(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_CLASS | (s & EVM_ADDR_MASK);
}

static inline void evm_set_buffer(evm_val_t *v, void *s) {
    *((uint64_t *)v) = INNER_TYPE_BUFFER | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline void evm_set_list(evm_val_t *v, void *s) {
    *((uint64_t *)v) = INNER_TYPE_LIST | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline void evm_set_foreign(evm_val_t *v, void *s) {
    *((uint64_t *)v) = INNER_TYPE_FOREIGN | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline void evm_set_invoke(evm_val_t *v, void *s) {
    *((uint64_t *)v) = INNER_TYPE_INVOKE | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline evm_val_t evm_mk_invoke( void *s) {
    return  INNER_TYPE_INVOKE | ((intptr_t)s & EVM_ADDR_MASK);
}

static inline int evm_is_invoke(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_INVOKE;
}

#endif

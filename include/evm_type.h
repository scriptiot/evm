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

/**
 * EVM虚拟机语言种类支持
 */
#define EVM_LANG_PY     1   /** python语法*/
#define EVM_LANG_JS     2   /** JavaScript语法*/
#define EVM_LANG_LUA    3   /** LUA语法*/
#define EVM_LANG_JSON   4   /** JSON语法*/
#define EVM_LANG_XML    5   /** XML语法*/
#define EVM_LANG_QML    6   /** QML语法*/
#define EVM_LANG_UOL    7   /** UOL语法*/

#define EVM_VER                   100  /** 虚拟机版本号*/
#define EVM_VAR_NAME_MAX_LEN      255  /** 虚拟机解析文件变量名称最大长度*/
#define EVM_FILE_NAME_LEN         255  /** 文件名称长度*/

#define EVM_LOAD_MODULE      0      /** 虚拟机加载模块*/
#define EVM_LOAD_MAIN        1      /** 虚拟机加载main*/

#define EVM_UNUSED(x) ((void) (x))
#define EVM_GC_TYPE_MASK(v)  (v & 0xff)

enum GC_TYPE
{
    GC_NONE,
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
    GC_MODULE,
    GC_MEMBER,
    GC_MEMBER_KEYS,
    GC_MEMBER_VALS,

    //NEVM GC TYPE
    GC_NEVM_ROOT,
    GC_NEVM_FUNCTION,
    GC_NEVM_CLASS,
    GC_NEVM_OBJECT,
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
    TYPE_INT,
    TYPE_FRAME,
    TYPE_FOREIGN,
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

/**
 * @brief evm_native_fn函数声明
 * @param e，虚拟机参数
 * @param self，绑定的内置函数
 * @param vc, 函数参数个数
 * @param v，函数参数指针
 * @return 内置函数对象
 */
typedef evm_val_t (*evm_native_fn)(void * e, evm_val_t * self, int vc, evm_val_t * v);

typedef struct evm_builtin_t{
    const char * name;
    evm_val_t v;
}evm_builtin_t;

typedef struct heap_t {
    int size;
    int free;
    void *base;
} heap_t;

typedef struct evm_const_pool_t{
    int count;
    evm_val_t * values;
    struct evm_const_pool_t * next;
}evm_const_pool_t;

typedef struct evm_t{
    uint8_t err;
    const char * err_arg;
    char * file_name;
    uint32_t file_name_len;

    char * var_name;
    int var_name_len;

    heap_t * heap;
    evm_val_t * sp;
    evm_val_t * sp_base;
    uint32_t stack_max_size;
    int stack_size;
    evm_val_t root;
    evm_val_t scope;

    int32_t native_cnt;
    intptr_t * native_symbals;
    evm_val_t * native_tbl;
    int32_t module_size;
    int32_t module_cnt;
    intptr_t * module_symbals;
    evm_val_t * module_tbl;

    evm_const_pool_t * string_pool;
    evm_const_pool_t * number_pool;

    evm_val_t number_object;
}evm_t;

typedef void (*evm_init_fn)(evm_t * e, evm_val_t * old_self, evm_val_t * new_self);
typedef void (*evm_destroy_fn)(evm_t * e, evm_val_t *self);

typedef union {
    evm_val_t   v;
    double      d;
} evm_valnum_t;

#define double_2_val(d) (((evm_valnum_t*)&(d))->v)

#define MAKE_TYPE(s, t) \
  ((evm_val_t)(s) << 63 | (evm_val_t) 0x7ff0 <<48 | (evm_val_t)(t) <<48)

#define INNER_TYPE_INFINITE        MAKE_TYPE(0, TYPE_NUMBER)
#define INNER_TYPE_INT             MAKE_TYPE(1, TYPE_INT)
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

#define INNER_TYPE_MASK            MAKE_TYPE(1, 0xF)
#define EVM_VAL_MASK               (~MAKE_TYPE(1, 0xF))

#define EVM_INT_TYPE_MASK          0x0F00000000000000

#define EVM_VAL_UNDEFINED       INNER_TYPE_UNDEFINED
#define EVM_VAL_NULL            INNER_TYPE_NULL
#define EVM_VAL_INFINITE        INNER_TYPE_INFINITE
#define EVM_VAL_TRUE            (INNER_TYPE_BOOLEAN | 1)
#define EVM_VAL_FALSE           (INNER_TYPE_BOOLEAN)

/**
 * @brief 判断值基本类型
 * @param v
 * @return
 */
static inline int evm_type(evm_val_t *v) {
    int type = (*v) >> 48;
    if ((type & 0x7ff0) != 0x7ff0) {
        return 0;
    } else {
        return type & 0xf;
    }
}
/**
 * @brief 值转浮点
 * @param v
 * @return
 */
static inline double evm_2_double(evm_val_t *v) {
    return ((evm_valnum_t*)v)->d;
}
/**
 * @brief 值转整数
 * @param v
 * @return
 */
static inline long evm_2_integer(evm_val_t *v) {
    return (long)((evm_valnum_t*)v)->d;
}
/**
 * @brief 值转指针
 * @param v
 * @return
 */
static inline intptr_t evm_2_intptr(evm_val_t *v) {
    return (intptr_t)(*v & EVM_VAL_MASK);
}
/**
 * @brief 值转布尔
 * @param v
 * @return
 */
static inline int evm_2_boolean(evm_val_t *v) {
    return evm_2_intptr(v);
}
/**
 * @brief 判断值是否为数字
 * @param v
 * @return
 */
static inline int evm_is_number(evm_val_t *v) {
    return (*v & INNER_TYPE_INFINITE) != INNER_TYPE_INFINITE;
}
/**
 * @brief 判断值是否为整数
 * @param v
 * @return
 */
static inline int evm_is_integer(evm_val_t *v){
    if(!evm_is_number(v)) return 0;
    if(evm_2_double(v) - evm_2_integer(v) == 0) return 1;
    return 0;
}
/**
 * @brief 判断值是否为堆字符串
 * @param v
 * @return
 */
static inline int evm_is_heap_string(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_STRING_H;
}
/**
 * @brief 判断值是否为外部字符串
 * @param v
 * @return
 */
static inline int evm_is_foreign_string(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_STRING_F;
}
/**
 * @brief 判断值是否为字符串
 * @param v
 * @return
 */
static inline int evm_is_string(evm_val_t *v) {
    return evm_is_heap_string(v) || evm_is_foreign_string(v);
}
/**
 * @brief 判断值是否为布尔值
 * @param v
 * @return
 */
static inline int evm_is_boolean(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_BOOLEAN;
}
/**
 * @brief 判断值是否为字符数组
 * @param v
 * @return
 */
static inline int evm_is_buffer(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_BUFFER;
}
/**
 * @brief 判断值是否为脚本函数
 * @param v
 * @return
 */
static inline int evm_is_script(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FUNC_SCRIPT;
}
/**
 * @brief 判断值是否为内置函数
 * @param v
 * @return
 */
static inline int evm_is_native(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FUNC_NATIVE;
}
/**
 * @brief 判断值是否为数组、列表
 * @param v
 * @return
 */
static inline int evm_is_list(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_LIST;
}
/**
 * @brief 判断值是否为外部指针
 * @param v
 * @return
 */
static inline int evm_is_foreign(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_FOREIGN;
}
/**
 * @brief 判断值是否为函数
 * @param v
 * @return
 */
static inline int evm_is_function(evm_val_t *v) {
    return evm_is_script(v) || evm_is_native(v);
}
/**
 * @brief 判断值是否为undefined
 * @param v
 * @return
 */
static inline int evm_is_undefined(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_UNDEFINED;
}
/**
 * @brief 判断值是否为空
 * @param v
 * @return
 */
static inline int evm_is_null(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_NULL;
}
/**
 * @brief 判断值是否为NaN
 * @param v
 * @return
 */
static inline int evm_is_nan(evm_val_t *v)
{
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_NAN;
}
/**
 * @brief 判断值是否为对象
 * @param v
 * @return
 */
static inline int evm_is_object(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_OBJECT;
}
/**
 * @brief 判断值是否为类
 * @param v
 * @return
 */
static inline int evm_is_class(evm_val_t *v) {
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_CLASS;
}
/**
 * @brief 值转字符串
 * @param v
 * @return
 */
const char *evm_2_string(evm_val_t *v);
/**
 * @brief 创建数字值
 * @param 浮点小数
 * @return
 */
static inline evm_val_t evm_mk_number(double d) {
    return double_2_val(d);
}
/**
 * @brief evm_mk_integer
 * @param v
 * @param type
 * @return
 */
static inline evm_val_t evm_mk_integer(int v, int type) {
    return INNER_TYPE_INT | v | ((evm_val_t)type << 52);
}
/**
 * @brief 创建外部字符串值
 * @param 字符串指针
 * @return
 */
static inline evm_val_t evm_mk_foreign_string(intptr_t s) {
    return INNER_TYPE_STRING_F | s;
}
/**
 * @brief 创建堆字符串值
 * @param 堆字符串对象
 * @return
 */
static inline evm_val_t evm_mk_heap_string(intptr_t s) {
    return INNER_TYPE_STRING_H | s;
}
/**
 * @brief 创建布尔值
 * @param 0或1
 * @return
 */
static inline evm_val_t evm_mk_boolean(int v) {
    return INNER_TYPE_BOOLEAN | (!!v);
}
/**
 * @brief 创建脚本函数值
 * @param v
 * @return
 */
static inline evm_val_t evm_mk_script(intptr_t s) {
    return INNER_TYPE_FUNC_SCRIPT | s;
}
/**
 * @brief 创建内置函数值
 * @param 函数地址
 * @return
 */
static inline evm_val_t evm_mk_native(intptr_t n) {
    return INNER_TYPE_FUNC_NATIVE | n;
}
/**
 * @brief 创建空值
 * @return
 */
static inline evm_val_t evm_mk_null(void) {
    return INNER_TYPE_NULL;
}
/**
 * @brief 创建undefined值
 * @return
 */
static inline evm_val_t evm_mk_undefined(void) {
    return INNER_TYPE_UNDEFINED;
}
/**
 * @brief 创建对象值
 * @param 对象指针
 * @return
 */
static inline evm_val_t evm_mk_object(void *ptr) {
    return INNER_TYPE_OBJECT | (intptr_t) ptr;
}
/**
 * @brief 创建字节数组值
 * @param 字节数组数组指针
 * @return
 */
static inline evm_val_t evm_mk_buffer(void *ptr) {
    return INNER_TYPE_BUFFER | (intptr_t) ptr;
}
/**
 * @brief 创建数组、列表值
 * @param 数组、列表对象
 * @return
 */
static inline evm_val_t evm_mk_list(void *ptr) {
    return INNER_TYPE_LIST | (intptr_t) ptr;
}
/**
 * @brief 创建外部对象值
 * @param 外部对象指针
 * @return
 */
static inline evm_val_t evm_mk_foreign(void *ptr) {
    return INNER_TYPE_FOREIGN | (intptr_t) ptr;
}
/**
 * @brief 创建类值
 * @param 类对象指针
 * @return
 */
static inline evm_val_t evm_mk_class(void *ptr) {
    return INNER_TYPE_CLASS | (intptr_t) ptr;
}
/**
 * @brief 创建true值
 * @return
 */
static inline evm_val_t evm_mk_true(void) {
    return EVM_VAL_TRUE;
}
/**
 * @brief 创建false值
 * @return
 */
static inline evm_val_t evm_mk_false(void) {
    return EVM_VAL_FALSE;
}
/**
 * @brief 创建NaN值
 * @return
 */
static inline evm_val_t evm_mk_nan(void)
{
    return INNER_TYPE_NAN;
}
/**
 * @brief 设置布尔值
 * @param v
 * @param 0或1
 */
static inline void evm_set_boolean(evm_val_t *v, int b) {
    *((uint64_t *)v) = INNER_TYPE_BOOLEAN | !!b;
}
/**
 * @brief 设置数字值
 * @param 值
 * @param 浮点小数
 */
static inline void evm_set_number(evm_val_t *v, double d) {
    *((double *)v) = d;
}
/**
 * @brief 设置NaN值
 * @param v
 */
static inline void evm_set_nan(evm_val_t *v)
{
    *((uint64_t *)v) = INNER_TYPE_NAN;
}
/**
 * @brief 设置值为外部字符串
 * @param v
 * @param 字符串指针
 */
static inline void evm_set_foreign_string(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_STRING_F | s;
}
/**
 * @brief 设置值为堆字符串
 * @param v
 * @param 堆字符串对象
 */
static inline void evm_set_heap_string(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_STRING_H | s;
}
/**
 * @brief 设置值为脚本函数
 * @param v
 * @param 函数对象指针
 */
static inline void evm_set_script(evm_val_t *v, intptr_t s) {
    *((uint64_t *)v) = INNER_TYPE_FUNC_SCRIPT | s;
}
/**
 * @brief 设置值为内置函数
 * @param v
 * @param 函数指针
 */
static inline void evm_set_native(evm_val_t *v, intptr_t f) {
    *((uint64_t *)v) = INNER_TYPE_FUNC_NATIVE | f;
}
/**
 * @brief 设置值为undefined
 * @param v
 */
static inline void evm_set_undefined(evm_val_t *v) {
    *((uint64_t *)v) = INNER_TYPE_UNDEFINED;
}
/**
 * @brief 设置值为null
 * @param v
 */
static inline void evm_set_null(evm_val_t *v) {
    *((uint64_t *)v) = INNER_TYPE_NULL;
}
/**
 * @brief 设置值为对象
 * @param v
 * @param 对象指针
 */
static inline void evm_set_object(evm_val_t *v, intptr_t d) {
    *((uint64_t *)v) = INNER_TYPE_OBJECT | d;
}
/**
 * @brief 设置值为类
 * @param v
 * @param 类对象指针
 */
static inline void evm_set_class(evm_val_t *v, intptr_t d) {
    *((uint64_t *)v) = INNER_TYPE_CLASS | d;
}
/**
 * @brief 设置值为字节数组
 * @param v
 * @param 数组指针
 */
static inline void evm_set_buffer(evm_val_t *v, void *b) {
    *((uint64_t *)v) = INNER_TYPE_BUFFER | (intptr_t)b;
}
/**
 * @brief 设置值为数组、列表
 * @param v
 * @param 数组、列表指针
 */
static inline void evm_set_list(evm_val_t *v, void *b) {
    *((uint64_t *)v) = INNER_TYPE_LIST | (intptr_t)b;
}
/**
 * @brief 设置值为外部对象
 * @param v
 * @param 外部对象指针
 */
static inline void evm_set_foreign(evm_val_t *v, void *b) {
    *((uint64_t *)v) = INNER_TYPE_FOREIGN | (intptr_t)b;
}

static inline int evm_cval_type(evm_val_t *v){
    if( (*v & INNER_TYPE_MASK) != INNER_TYPE_INT ) return 0;
    int type = (*v) >> 44;
    return type & 0xf;
}

static inline int evm_is_cval(evm_val_t *v){
    return (*v & INNER_TYPE_MASK) == INNER_TYPE_INT;
}

static inline evm_val_t evm_mk_cval(int type, uint32_t i){
    return INNER_TYPE_INT | i | ((evm_val_t)type << 44);
}

static inline int evm_set_cval(evm_val_t *v, int type, uint32_t i){
    *((uint64_t *)v) = INNER_TYPE_INT | i | ((evm_val_t)type << 44);
    return ec_ok;
}

#endif

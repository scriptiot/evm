/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot
**  Licence: 个人免费，企业授权
****************************************************************************/
/** @file      evm.h
 * @brief      Copyright (c) 2019, 武汉市字节码科技有限公司（Wuhan ByteCode Technology Co.,Ltd)
 * @copyright  scripiot
 */
#ifndef EVM_H
#define EVM_H

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

/**
 * EVM虚拟机语言种类支持
 */
#define EVM_LANG_PY     1   /** python语法*/
#define EVM_LANG_JS     2   /** JavaScript语法*/
#define EVM_LANG_LUA    3   /** LUA语法*/
#define EVM_LANG_JSON   4   /** JSON语法*/
#define EVM_LANG_XML    5   /** XML语法*/
#define EVM_LANG_QML    6   /** QML语法*/

#define EVM_VER                   100  /** 虚拟机版本号*/
#define EVM_VAR_NAME_MAX_LEN      255  /** 虚拟机解析文件变量名称最大长度*/
#define EVM_FILE_NAME_LEN         255  /** 文件名称长度*/
#define EVM_HEAP_SIZE             (10 * 1024) /** 虚拟机堆大小*/
#define EVM_STACK_SIZE            (10 * 1024) /** 虚拟机栈大小*/
#define EVM_MODULE_SIZE           10          /** 虚拟机栈内置模块数量大小*/

#define EVM_LOAD_MODULE      0      /** 虚拟机加载模块*/
#define EVM_LOAD_MAIN        1      /** 虚拟机加载main*/

#define EVM_UNUSED(x) ((void) (x))



typedef uint64_t evm_val_t;
typedef uint32_t evm_err_t;

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
    GC_BUFFER16_U,
    GC_BUFFER32_U,
    GC_BUFFER64_U,
    GC_BUFFER_FLOAT,
    GC_BUFFER_DOUBLE,
    GC_LOCAL,
    GC_STRING,
    GC_NUMBER,
    GC_BOOLEAN,
    GC_TUPLE,
    GC_MODULE,
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
};

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
    int file_name_len;

    char * var_name;
    int var_name_len;

    heap_t * heap;
    evm_val_t * sp;
    evm_val_t * sp_base;
    int stack_size;
    evm_val_t root;
    evm_val_t scope;

    int32_t native_cnt;
    intptr_t * native_symbals;
    evm_val_t * native_tbl;
    int32_t module_size;
    int32_t module_cnt;
    intptr_t * module_symbals;
    intptr_t * module_tbl;

    evm_const_pool_t * string_pool;
    evm_const_pool_t * number_pool;

    evm_val_t number_object;
}evm_t;

typedef void (*evm_init_fn)(evm_t * e, evm_val_t * old_self, evm_val_t * new_self);
typedef void (*evm_destroy_fn)(evm_t * e, evm_val_t *self);
/**
 * @brief 添加内置函数列表
 * @param e，虚拟机
 * @param n，内置函数列表
 * @return 错误码
 */
evm_err_t evm_native_add(evm_t * e, evm_builtin_t * n);
/**
 * @brief 调用垃圾回收
 * @param e，虚拟机参数
 */
int evm_heap_gc(evm_t *e);
/**
 * @brief 触发gc保护外部对象
 * @param e
 * @param v
 */
void evm_gc_protect(evm_t *e , evm_val_t * v);
/**
 * @brief 虚拟机设置错误码
 * @param e，虚拟机
 * @param err，错误码
 * @param arg，错误内容字符串
 * @return
 */
int evm_set_err(evm_t * e, int err, const char *arg);
/**
 * @brief 通过index查找字符串
 * @param e，虚拟机
 * @param index，索引
 * @return 字符串，若找不到，返回NULL
 */
char *evm_string_get(evm_t * e, uint16_t key);
/**
 * @brief evm_string_pool_get
 * @param e
 * @param key
 * @return
 */
evm_val_t * evm_string_pool_get(evm_t *e, uint16_t key);
/**
 * @brief evm_number_get
 * @param e
 * @param key
 * @return
 */
double evm_number_get(evm_t *e, uint16_t key);
/**
 * @brief 通过字符串名称查找该名称的索引值
 * @param e,虚拟机
 * @param str，字符串
 * @return
 */
int evm_str_lookup(evm_t * e, const char *str);
/**
 * @brief 插入字符串到常量池
 * @param e,虚拟机
 * @param str，字符串
 * @param alloc，1表示复制字符串对象，0表示不复制
 * @return 常量池索引位置
 */
uint16_t evm_str_insert(evm_t *e, const char *str, int alloc);
/**
 * @brief 插入数字到常量池
 * @param e,虚拟机
 * @param num，数字
 * @return 常量池索引位置
 */
int evm_number_insert(evm_t *e, double num);
/**
 * @brief 创建字节数组
 * @param e,虚拟机参数
 * @param len,数组长度
 * @return 字节数组对象
 */
evm_val_t *evm_buffer_create(evm_t *e, int len);
/**
 * @brief evm_buffer_addr
 * @param o
 * @return
 */
uint8_t * evm_buffer_addr(evm_val_t * o);
/**
 * @brief 获取字节数组长度
 * @param v,字节数组对象
 * @return 字节数组长度
 */
int evm_buffer_len(evm_val_t * o);
/**
 * @brief 字节数组内容设置
 * @param o，字节数组对象
 * @param index，索引
 * @param len，长度
 * @param buffer，写入内容缓存数组
 * @return 虚拟机错误码
 */
evm_err_t evm_buffer_set(evm_val_t * o, uint8_t * buffer, uint32_t index, uint32_t len);
/**
 * @brief 创建堆字符串
 * @param e，虚拟机参数
 * @param len，字符串长度
 * @return 字符串对象，如果失败，返回UNDEFINED
 */
evm_val_t *evm_heap_string_create(evm_t *e, char *str, int len);
/**
 * @brief 获取堆字符串长度
 * @param o，堆字符串对象
 * @return 字符串长度
 */
int evm_string_len(evm_val_t * o);
/**
 * @brief 获取堆字符串实际缓存地址
 * @param v
 * @return
 */
char* evm_heap_string_addr(evm_val_t * v);
/**
 * @brief 堆字符串内容设置
 * @param o，堆字符串对象
 * @param buffer，写入内容缓存数组
 * @param index，索引
 * @param len，长度
 * @return 虚拟机错误码
 */
evm_err_t evm_heap_string_set(evm_val_t * o, uint8_t * buffer, uint32_t index, uint32_t len);
/**
 * @brief 对象复制
 * @param e，虚拟机
 * @param type，对象类型设置
 * @param o，被复制的对象
 * @return 复制后新的对象
 */
evm_val_t *evm_object_duplicate(evm_t * e, int type, evm_val_t * o);
/**
 * @brief 获取对象的parent对象
 * @param e，虚拟机参数
 * @return
 */
evm_val_t* evm_get_parent(evm_t *e, evm_val_t o);
/**
 * @brief 对象获取外部数据
 * @param o
 * @return
 */
intptr_t evm_object_get_ext_data(evm_val_t * o);
/**
 * @brief 对象设置外部数据
 * @param o
 * @param v
 */
void evm_object_set_ext_data(evm_val_t *o, intptr_t v);
/**
 * @brief 对象获取init函数
 * @param o
 * @return
 */
evm_init_fn evm_object_get_init(evm_val_t * o);
/**
 * @brief 对象设置init函数
 * @param o
 * @param v
 */
void evm_object_set_init(evm_val_t *o, evm_init_fn fn);
/**
 * @brief 对象设置destroy函数，当对象被当做垃圾回收时，触发该函数
 * @param o
 * @return
 */
evm_destroy_fn evm_object_get_destroy(evm_val_t * o);
/**
 * @brief 对象获取destroy函数，当对象被当做垃圾回收时，触发该函数
 * @param o
 * @param v
 */
void evm_object_set_set_destroy(evm_val_t *o, evm_destroy_fn fn);
/**
 * @brief 设置对象继承的父类对象
 * @param e，虚拟机参数
 * @param parent，父类对象
 * @return
 */
void evm_set_parent(evm_val_t *o, evm_val_t *parent);
/**
 * @brief 创建新的对象
 * @param e，虚拟机参数
 * @param type，设置对象类型
 * @param prop_len，设置成员长度
 * @param attr_len，设置属性长度
 * @return
 */
evm_val_t *evm_object_create(evm_t * e, int type, int prop_len, int attr_len);
/**
 * @brief 创建带有外部数据的新对象
 * @param e
 * @param type
 * @param ext_data
 * @return
 */
evm_val_t *evm_object_create_ext_data(evm_t * e, int type, intptr_t ext_data);
/**
 * @brief 通过类来创建对象
 * @param e，虚拟机参数
 * @param o，类对象
 * @return 创建的新对象
 */
evm_val_t * evm_object_create_by_class(evm_t * e, int type, evm_val_t * o);
/**
 * @brief 去除对象引用，强制垃圾回收
 * @param e
 * @param o
 */
void evm_object_deref(evm_t * e, evm_val_t * o);
/**
 * @brief 通过字符串名称对象查找成员
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，字符串名称
 * @param depth，depth大于0表示查找范围包括父类，否则不包括父类
 * @return 成员，查找失败则返回undefined
 */
evm_val_t * evm_prop_get(evm_t *e, evm_val_t * o, const char* key, int depth);
/**
 * @brief evm_prop_get_by_key
 * @param e
 * @param obj
 * @param key
 * @param depth
 * @return
 */
evm_val_t * evm_prop_get_by_key(evm_t * e, evm_val_t *obj, uint32_t key, int depth);
/**
 * @brief 通过成员名称索引，获取成员在对象中索引
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，名称索引值
 * @return 成员在对象中的索引位置
 */
int evm_prop_key_index(evm_t * e, evm_val_t * o, uint32_t key);
/**
 * @brief 通过索引位置获取对象成员
 * @param e
 * @param o
 * @param index
 * @return
 */
evm_val_t * evm_prop_get_by_index(evm_t * e, evm_val_t * o, int index);
/**
 * @brief 按顺序添加对象成员
 * @param e，虚拟机参数
 * @param o，对象
 * @param index, 索引
 * @param key，字符串名称
 * @param v，成员值
 * @return 错误码，如果当前成员内容已满，则返回错误
 */
evm_err_t evm_prop_set(evm_t * e, evm_val_t * o, uint32_t index, const char *key, evm_val_t v);
/**
 * @brief evm_prop_set_key_value
 * @param e
 * @param o
 * @param index
 * @param key
 * @param v
 * @return
 */
evm_err_t evm_prop_set_key_value(evm_t * e, evm_val_t * o, uint32_t index, uint16_t key, evm_val_t v);
/**
 * @brief 对象成员设置
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，字符串名称
 * @param v，成员值
 * @return 错误码
 */
evm_err_t evm_prop_set_value(evm_t * e, evm_val_t * o, const char* key, evm_val_t v);
/**
 * @brief 对象追加成员
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，字符串名称
 * @param v，值
 * @return 错误码
 */
evm_err_t evm_prop_append(evm_t * e, evm_val_t * o, char * key, evm_val_t * v);
/**
 * @brief 对象通过索引追加成员
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，字符串索引
 * @param v，值
 * @return 错误码
 */
evm_err_t evm_prop_append_with_key(evm_t * e, evm_val_t * o, int key, evm_val_t * v);
/**
 * @brief 获取对象成员长度
 * documenting functions.
 * @param o，对象
 * @return 长度
 */
evm_err_t evm_prop_set_value_by_index(evm_t * e, evm_val_t *o, uint32_t index, evm_val_t v);
/**
 * @brief 通过索引获取成员的名称索引值
 * @param e，虚拟机
 * @param o，对象
 * @param index，索引
 * @return 名称索引值，通过evm_string_get获取索引对应的字符串内容
 */
uint32_t evm_prop_get_key_by_index(evm_t * e, evm_val_t * o, int index);
/**
 * @brief 通过索引设置成员的名称索引值
 * @param e，虚拟机参数
 * @param o，对象
 * @param index，索引
 * @param key，字符串索引值
 */
void evm_prop_set_key_by_index(evm_t * e, evm_val_t * o, int index, uint32_t key);
/**
 * @brief 对象成员获取长度
 * @param o，对象
 * @return
 */
int evm_prop_len(evm_val_t * o);
/**
 * @brief 获取对象属性长度
 * @param o，对象
 * @return 长度
 */
int evm_attr_len(evm_val_t * o);
/**
 * @brief 创建内置函数对象
 * @param e，虚拟机参数
 * @param fn，绑定的内置函数
 * @param prop_len，成员长度
 * @param attr_len，属性长度
 * @return 内置函数对象
 */
evm_val_t *evm_native_function_create(evm_t *e, evm_native_fn fn, int attr_len);
/**
 * @brief 创建数组、列表
 * @param e，虚拟机参数
 * @param type，类型
 * @param count，长度
 * @return 数组、列表对象
 */
evm_val_t * evm_list_create(evm_t * e, int type, uint16_t len);
/**
 * @brief 数组对象增加内容
 * @param e，虚拟机参数
 * @param o，数组对象
 * @param len，增加长度
 * @param v，内容
 * @return 错误码
 */
evm_err_t evm_list_push(evm_t * e, evm_val_t * o, int len, evm_val_t *v);
/**
 * @brief 数组删除最后一个内容，并返回该内容
 * @param e，虚拟机
 * @param o，数组对象
 * @return 被删除的内容,若删除失败，则返回undefined
 */
evm_val_t *evm_list_pop(evm_t * e, evm_val_t * o);
/**
 * @brief 获取数组长度
 * @param o，数组对象
 * @return 数组长度
 */
int evm_list_len(evm_val_t * o);
/**
 * @brief 数组设置
 * @param e，虚拟机参数
 * @param o，数组对象
 * @param index，索引
 * @param v，值
 * @return 错误码
 */
evm_err_t evm_list_set(evm_t * e, evm_val_t * o, uint32_t index, evm_val_t v);
/**
 * @brief 对象创建成员
 * @param e
 * @param obj
 * @param 成员长度
 * @return
 */
evm_err_t evm_prop_create(evm_t * e, evm_val_t *obj, int len);
/**
 * @brief 数组获取内容
 * @param e，虚拟机参数
 * @param o，数组对象
 * @param index，索引
 * @return 错误码
 */
evm_val_t * evm_list_get(evm_t * e, evm_val_t * o, uint32_t index);
/**
 * @brief 对象属性创建
 * @param e，虚拟机
 * @param o, 对象
 * @param len，属性长度
 * @return 错误码
 */
evm_err_t evm_attr_create(evm_t * e, evm_val_t * o, int len);
/**
 * @brief 设置对象属性
 * @param e，虚拟机
 * @param o，对象
 * @param index，索引
 * @param name，属性名称
 * @param v，值
 * @return 增加后的属性索引，增加失败返回-1，超过属性长度也返回-1
 */
evm_err_t evm_attr_set(evm_t * e, evm_val_t * o, uint32_t index, const char *name, evm_val_t v);
/**
 * @brief 通过索引值设置属性
 * @param e，虚拟机参数
 * @param o，对象
 * @param index，索引
 * @param v，设置值
 * @return
 */
evm_err_t evm_attr_set_by_index(evm_t * e, evm_val_t * o, uint32_t index, evm_val_t v);
/**
 * @brief 获取对象属性
 * @param e，虚拟机
 * @param o，对象
 * @param name，名称
 * @return 属性值，未找到返回NULL
 */
evm_val_t* evm_attr_get(evm_t * e, evm_val_t * o, const char *name);
/**
 * @brief 通过名称索引值寻找对象属性
 * @param e，虚拟机
 * @param o，对象
 * @param key，名称索引值，可以通过evm_str_lookup寻找
 * @return 属性值，未找到返回NULL
 */
evm_val_t * evm_attr_get_by_index(evm_t * e, evm_val_t * o, uint32_t index);

/**
 * @brief 追加对象属性
 * @param e，虚拟机
 * @param o，对象
 * @param name，属性字符串名称
 * @param v，值
 * @return 错误码
 */
evm_err_t evm_attr_append(evm_t * e, evm_val_t * o, char* name, evm_val_t v);
/**
 * @brief 对象获取内置函数
 * @param o，对象
 * @return
 */
evm_native_fn evm_script_get_native(evm_val_t *o);

/**
 * @brief 脚本函数设置内置函数
 * @param o
 * @param fn
 */
void evm_script_set_native(evm_val_t *o, evm_native_fn fn);
/**
 * @brief 对象获取GC类型
 * @param o，对象
 * @return
 */
int evm_get_gc_type(evm_val_t * o);
/**
 * @brief 设置对象GC类型
 * @param o
 * @param type
 * @return
 */
int evm_set_gc_type(evm_val_t *o, int type);
/**
 * @brief 创建类
 * @param e，虚拟机
 * @param props_len,成员长度
 * @param props_len,属性长度
 * @return 类对象
 */
evm_val_t * evm_class_create(evm_t * e, evm_native_fn fn, evm_builtin_t * props, evm_builtin_t * attrs);
/**
 * @brief 脚本函数设置内置函数
 * @param o
 * @param fn
 */
void evm_class_set_native(evm_val_t *o, evm_native_fn fn);
/**
 * @brief evm_class_get_native
 * @param o
 * @return
 */
evm_native_fn evm_class_get_native(evm_val_t *o);
/**
 * @brief evm_object_set_scope
 * @param o
 * @param s
 */
void evm_object_set_scope(evm_val_t *o, evm_val_t *s);
/**
 * @brief 创建内置模块
 * @param e，虚拟机
 * @param name，名称
 * @param n，内置成员列表
 * @return 内置对象
 */
evm_val_t evm_module_create(evm_t * e, const char* name, evm_builtin_t *n);
/**
 * @brief 添加内置模块
 * @param e，虚拟机
 * @param name，名称
 * @param v，对象
 * @return 错误码
 */
evm_err_t evm_module_add(evm_t * e, const char* name, evm_val_t * v);
/**
 * @brief 获取内置模块
 * @param e，虚拟机
 * @param name，名称
 * @return 模块对象
 */
evm_val_t evm_module_get(evm_t * e, const char* name);
/*** 虚拟机相关函数 ***/
/**
 * @brief 虚拟机启动
 * @param e，虚拟机参数
 * @return 错误码
 */
evm_err_t evm_start(evm_t * e);
/**
 * @brief 虚拟机同步调用回调函数
 * @param e，虚拟机
 * @param scope，作用域，一般是函数
 * @param p_this，当前对象
 * @param args，参数
 * @param argc，参数长度
 * @return
 */
evm_val_t evm_run_callback(evm_t * e, evm_val_t * scope, evm_val_t *p_this, evm_val_t *args, int argc);
/**
 * @brief 虚拟机启动前加载
 * @param e，虚拟机
 * @param path，文件路径
 * @return 错误码
 */
evm_err_t evm_boot(evm_t *e, char *path);
/**
 * @brief 虚拟机初始化
 * @param e, 虚拟机
 * @param heap_size，堆大小
 * @param stack_size，栈大小
 * @param const_table_size，常量池大小
 * @param module_size，内置模块数量大小
 * @return 错误码
 */
evm_err_t evm_init(evm_t * e, int heap_size, int stack_size, int module_size, int var_name_len, int file_name_len);
/**
 * @brief 虚拟机注销
 * @param e,虚拟机
 */
void evm_deinit(evm_t * e);
/**
 * @brief 错误码打印
 * @param e，虚拟机
 */
void evm_errcode_print(evm_t *e);

extern void (*evm_print)(const char *fmt, ...);
extern void * (*evm_malloc)(int size);
extern void (*evm_free)(void * mem);

static inline void evm_assert_fail (const char *assertion, const char *file, const char *function, const uint32_t line){
    evm_print ("AssertionError: '%s' failed at %s(%s):%lu.\n",
                       assertion,
                       file,
                       function,
                       (unsigned long) line);

    while(1);
}

#define EVM_ASSERT(x) \
  do \
  { \
    if (!x) \
    { \
      evm_assert_fail (#x, __FILE__, __func__, __LINE__); \
    } \
  } while (0)



/*** 外部实现接口 ***/
void evm_register_print(intptr_t fn);
void evm_register_free(intptr_t fn);
void evm_register_malloc(intptr_t fn);
void evm_register_file_load(intptr_t fn);
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
 * @brief 判断值是否是数字
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

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
#ifndef EVM_H
#define EVM_H

#include "evm_type.h"

/*** 字符串常量操作函数 ***/
EVM_API evm_hash_t evm_str_lookup(evm_t *e, const char *str, evm_err_t *ok);
EVM_API evm_hash_t evm_str_insert(evm_t *e, const char *str, int alloc);
EVM_API const char *evm_string_get(evm_t * e, evm_hash_t key);

/*** 数字常量操作函数 ***/
EVM_API evm_hash_t evm_number_insert(evm_t *e, double num);
EVM_API double evm_number_get(evm_t *e, uint16_t key);

/*** 字符串对象操作函数 ***/
EVM_API evm_val_t *evm_heap_string_create(evm_t *e, const char *str, uint32_t len);
EVM_API char *evm_heap_string_addr(evm_val_t * v);
EVM_API uint32_t evm_string_len(evm_val_t * o);
EVM_API evm_err_t evm_heap_string_set(evm_t *e, evm_val_t *o, uint8_t *buffer, uint32_t index, uint32_t len);

/*** 字节数组对象操作函数 ***/
EVM_API evm_val_t *evm_buffer_create(evm_t *e, uint32_t len);
EVM_API evm_val_t *evm_buffer_create_ex(evm_t *e, evm_type_t gc_type, uint32_t len);
EVM_API uint32_t evm_buffer_get_count(evm_t *e, evm_val_t *o);
EVM_API evm_err_t evm_buffer_set_count(evm_t *e, evm_val_t *o, uint32_t count);
EVM_API uint8_t *evm_buffer_addr(evm_val_t *o);
EVM_API uint32_t evm_buffer_len(evm_val_t *o);
EVM_API evm_err_t evm_buffer_set(evm_t *e, evm_val_t *o, uint8_t * buffer, uint32_t index, uint32_t len);

/*** 列表对象操作函数 ***/
EVM_API evm_val_t *evm_list_create(evm_t *e, evm_type_t type, uint32_t len);
EVM_API evm_err_t evm_list_push(evm_t *e, evm_val_t *o, uint32_t len, evm_val_t *v);
EVM_API evm_val_t *evm_list_pop(evm_t *e, evm_val_t *o);
EVM_API uint32_t evm_list_len(evm_val_t *o);
EVM_API evm_err_t evm_list_set(evm_t *e, evm_val_t *o, uint32_t index, evm_val_t v);
EVM_API evm_val_t *evm_list_get(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API void evm_list_set_len(evm_val_t *v, uint32_t len);
EVM_API void evm_list_set_count(evm_val_t *v, uint32_t count);
EVM_API uint32_t evm_list_get_count(evm_val_t *v);
EVM_API evm_err_t evm_list_append(evm_t * e, evm_val_t * obj, evm_val_t v);
EVM_API evm_val_t *evm_list_duplicate(evm_t * e, evm_val_t *o);

/*** 对象操作函数 ***/
EVM_API evm_val_t *evm_object_create(evm_t *e, evm_type_t type, uint32_t prop_len, uint32_t attr_len);
EVM_API evm_val_t *evm_object_create_ext_data(evm_t *e, evm_type_t type, intptr_t ext_data);
EVM_API evm_val_t *evm_object_create_by_class(evm_t *e, evm_type_t type, evm_val_t *o);
EVM_API evm_val_t *evm_native_function_create(evm_t *e, evm_object_native_t *native, uint32_t attr_len);
EVM_API evm_val_t *evm_object_duplicate(evm_t *e, evm_type_t type, evm_val_t *o);
EVM_API evm_hash_t evm_object_get_hash(evm_val_t *o);
EVM_API void evm_object_set_native(evm_val_t *o, evm_object_native_t *native);
EVM_API evm_object_native_t *evm_object_get_native(evm_val_t *o);
EVM_API void evm_object_set_count(evm_val_t *v, uint32_t count);
EVM_API uint32_t evm_object_get_count(evm_val_t *v);
EVM_API intptr_t evm_object_get_ext_data(evm_val_t *o);
EVM_API void evm_object_set_ext_data(evm_val_t *o, intptr_t v);

/*** 成员操作函数 ***/
EVM_API evm_err_t evm_prop_create(evm_t *e, evm_val_t *obj, uint32_t len);
EVM_API evm_val_t *evm_prop_get(evm_t *e, evm_val_t *o, const char* key, int depth);
EVM_API evm_val_t *evm_prop_get_by_key(evm_t *e, evm_val_t *obj, evm_hash_t key, int depth);
EVM_API evm_val_t *evm_prop_get_by_index(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API evm_err_t evm_prop_set(evm_t *e, evm_val_t *o, uint32_t index, const char *key, evm_val_t v);
EVM_API evm_err_t evm_prop_set_key_value(evm_t *e, evm_val_t *o, uint32_t index, evm_hash_t key, evm_val_t v);
EVM_API evm_err_t evm_prop_set_value(evm_t *e, evm_val_t *o, const char* key, evm_val_t v);
EVM_API evm_err_t evm_prop_append(evm_t *e, evm_val_t *o, const char *key, evm_val_t v);
EVM_API evm_err_t evm_prop_append_with_key(evm_t *e, evm_val_t * o, evm_hash_t key, evm_val_t v);
EVM_API evm_err_t evm_prop_set_value_by_index(evm_t *e, evm_val_t *o, uint32_t index, evm_val_t v);
EVM_API evm_hash_t evm_prop_get_key_by_index(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API void evm_prop_set_key_by_index(evm_t *e, evm_val_t *o, uint32_t index, evm_hash_t key);
EVM_API uint32_t evm_prop_len(evm_val_t * o);

/*** 属性成员操作函数 ***/
EVM_API evm_err_t evm_attr_create(evm_t *e, evm_val_t *o, uint32_t len);
EVM_API evm_err_t evm_attr_set(evm_t *e, evm_val_t *o, uint32_t index, const char *name, evm_val_t v);
EVM_API evm_err_t evm_attr_set_key_value(evm_t *e, evm_val_t *o, uint32_t index, evm_hash_t key, evm_val_t v);
EVM_API evm_err_t evm_attr_set_by_index(evm_t *e, evm_val_t *o, uint32_t index, evm_val_t v);
EVM_API evm_val_t *evm_attr_get(evm_t *e, evm_val_t *o, const char *name);
EVM_API evm_hash_t evm_attr_get_key_by_index(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API evm_val_t *evm_attr_get_by_index(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API evm_val_t *evm_attr_get_by_key(evm_t *e, evm_val_t *obj, evm_hash_t key);
EVM_API evm_err_t evm_attr_append(evm_t *e, evm_val_t *o, const char *name, evm_val_t v);
EVM_API evm_err_t evm_attr_append_with_key(evm_t *e, evm_val_t *obj, evm_hash_t key, evm_val_t v);
EVM_API uint32_t evm_attr_len(evm_val_t *o);

/*** 模块操作函数 ***/
EVM_API evm_val_t evm_module_create(evm_t *e, const char* name, evm_builtin_t *n);
EVM_API evm_err_t evm_module_add(evm_t *e, const char* name, evm_val_t *v);
EVM_API evm_val_t *evm_module_get(evm_t *e, const char* name);

/*** 其它操作函数 ***/
EVM_API evm_err_t evm_is_prop_key_valid(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API evm_err_t evm_is_attr_key_valid(evm_t *e, evm_val_t *o, uint32_t index);
EVM_API evm_err_t evm_native_add(evm_t * e, evm_builtin_t * n);
EVM_API int evm_heap_gc(evm_t *e);
EVM_API evm_err_t evm_set_err(evm_t * e, evm_err_t err, const char *arg);
EVM_API evm_val_t* evm_get_parent(evm_t *e, evm_val_t *o);
EVM_API void evm_set_parent(evm_t *e, evm_val_t *o, evm_val_t *parent);
EVM_API evm_type_t evm_get_gc_type(evm_t *e, evm_val_t * o);
EVM_API evm_err_t evm_set_gc_type(evm_t *e, evm_val_t *o, evm_type_t type);
EVM_API evm_val_t * evm_class_create(evm_t * e, evm_object_native_t *fn, evm_builtin_t * props, evm_builtin_t * attrs);
EVM_API void evm_class_set_native(evm_val_t *o, evm_native_fn fn);
EVM_API void evm_object_set_scope(evm_val_t *o, evm_val_t *s);
EVM_API evm_val_t evm_object_get_scope(evm_val_t *o);

/*** 虚拟机相关函数 ***/
EVM_API evm_err_t evm_start(evm_t * e);
EVM_API evm_val_t evm_run_callback(evm_t * e, evm_val_t * scope, evm_val_t *p_this, evm_val_t *args, uint32_t argc);
EVM_API evm_err_t evm_boot(evm_t *e, char *path);
EVM_API evm_err_t evm_init(evm_t * e, uint32_t heap_size, uint32_t stack_size, uint32_t var_name_len, uint32_t file_name_len);
EVM_API void evm_deinit(evm_t * e);
EVM_API void evm_errcode_print(evm_t *e);
EVM_API evm_err_t evm_executable_load(evm_t *e, uint8_t * buf);
EVM_API evm_err_t evm_executable_run(evm_t *e);
EVM_API evm_err_t evm_executable_write(evm_t *e, char * path, uint8_t * buf, uint32_t * buf_len, int lang_type);
EVM_API evm_err_t evm_repl_run(evm_t * e, uint16_t number_of_variables, int language);
EVM_API evm_err_t nevm_start(evm_t *e);
EVM_API evm_val_t evm_run_script(evm_t * e, const char * full_path);
EVM_API evm_val_t evm_run_eval(evm_t * e,  evm_val_t *object, char * string,int lang_type);
EVM_API evm_val_t evm_module_load(evm_t * e, const char * full_path, int8_t refresh_needed);
EVM_API evm_val_t evm_run_string(evm_t * e, const char * content, int lang_type, int mode);

/*** 外部实现接口 ***/
EVM_API void evm_register_print(intptr_t fn);
EVM_API void evm_register_free(intptr_t fn);
EVM_API void evm_register_malloc(intptr_t fn);
EVM_API void evm_register_file_load(intptr_t fn);
EVM_API void evm_register_lock(void *fn);
EVM_API void evm_register_unlock(void *fn);
EVM_API char evm_repl_tty_read(evm_t * e);
EVM_API int (*evm_print)(const char *fmt, ...);
EVM_API void *(*evm_malloc)(size_t size);
EVM_API void (*evm_free)(void * mem);
EVM_API void (*evm_lock)(evm_t *e);
EVM_API void (*evm_unlock)(evm_t *e);

/*** 虚拟机栈操作 ***/
EVM_API void evm_push_null(evm_t * e);
EVM_API void evm_push_undefined(evm_t * e);
EVM_API void evm_push_number(evm_t * e, double v);
EVM_API void evm_push_integer(evm_t * e, int32_t v);
EVM_API void evm_push_boolean(evm_t * e, int v);
EVM_API evm_val_t *evm_push_buffer(evm_t * e, uint8_t * buf, uint32_t len);
EVM_API evm_val_t * evm_push_foreign_string(evm_t * e, const char * s);
EVM_API evm_val_t * evm_push_heap_string(evm_t * e, const char * s);
EVM_API void evm_push_value(evm_t *e, evm_val_t v);
EVM_API void evm_pop(evm_t *e);
EVM_API evm_val_t *evm_prop_push_with_key(evm_t * e, evm_val_t *obj, evm_hash_t key, evm_val_t *v);
EVM_API evm_val_t *evm_prop_push(evm_t * e, evm_val_t *o, const char *key, evm_val_t *v);
EVM_API evm_val_t *evm_list_pushvalue(evm_t * e, evm_val_t * obj, uint32_t index, evm_val_t *v);

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

#endif

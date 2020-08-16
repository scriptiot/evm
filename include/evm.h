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
 * @brief 通过key索引查找字符串
 * @param e，虚拟机
 * @param key，索引
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
int evm_str_insert(evm_t *e, const char *str, int alloc);
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
 * @brief 创建带有类型的buffer对象
 * @param e
 * @param gc类型
 * @param 长度
 * @return
 */
evm_val_t * evm_buffer_create_ex(evm_t *e, int gc_type, int len);
/**
 * @brief 获取buffer对象的数据地址
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
 * @brief 对象设置注销函数
 * @param o
 * @param destroy，注销函数
 */
void evm_object_set_destroy(evm_val_t *o, evm_native_fn destroy);
/**
 * @brief 获取对象hash值
 * @param o
 * @return hash值
 */
uint32_t evm_object_get_hash(evm_val_t *o);
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
 * @return 正确返回ec_ok，错误返回ec_index
 */
evm_err_t evm_prop_set(evm_t * e, evm_val_t * o, uint32_t index, const char *key, evm_val_t v);
/**
 * @brief 通过索引index，设置对象成员的key和value
 * @param e，虚拟机
 * @param o，对象
 * @param index，索引
 * @param key，字符串key值
 * @param v，成员值
 * @return 正确返回ec_ok，错误返回ec_index
 */
evm_err_t evm_prop_set_key_value(evm_t * e, evm_val_t * o, uint32_t index, uint16_t key, evm_val_t v);
/**
 * @brief 对象成员设置
 * @param e，虚拟机参数
 * @param o，对象
 * @param key，字符串名称
 * @param v，成员值
 * @return 正确返回ec_ok，错误返回ec_key
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
 * @brief 获取对象成员个数长度
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
 * @param attr_len，属性长度
 * @return 内置函数对象
 */
evm_val_t *evm_native_function_create(evm_t *e, evm_native_fn fn, int attr_len);
/**
 * @brief 创建数组、列表
 * @param e，虚拟机参数
 * @param type，类型
 * @param len，长度
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
 * @brief 设置对象属性
 * @param e
 * @param o
 * @param index
 * @param key
 * @param v
 * @return
 */
evm_err_t evm_attr_set_key_value(evm_t * e, evm_val_t * o, uint32_t index, uint16_t key, evm_val_t v);
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
 * @brief 获取对象属性
 * @param e
 * @param obj
 * @param key
 * @return
 */
evm_val_t* evm_attr_get_by_key(evm_t * e, evm_val_t *obj, int key);
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
/**
 * @brief 虚拟机加载字节码执行文件
 * @param e
 * @param buf，字节码二进制
 * @return 成功返回ec_ok,错误返回ec_err
 */
evm_err_t evm_executable_load(evm_t *e, uint8_t * buf);
/**
 * @brief 虚拟机运行字节码文件
 * @param e
 * @return
 */
evm_err_t evm_executable_run(evm_t *e);
/**
 * @brief 写字节码二进制文件
 * @param e
 * @param path，脚步文件路径
 * @param buf，写入文件缓存
 * @param buf_len，写入文件内容长度
 * @return 成功返回ec_ok,错误返回ec_err
 */
evm_err_t evm_executable_write(evm_t *e, char * path, uint8_t * buf, uint32_t * buf_len);
/**
 * @brief 交互式编程运行
 * @param e
 * @param number_of_variables
 * @param language
 * @return
 */
evm_err_t evm_repl_run(evm_t * e, uint16_t number_of_variables, int language);
/**
 * @brief 启动nano版本虚拟机，运行driverscript语言
 * @param e
 * @return
 */
evm_err_t nevm_start(evm_t *e);
extern char evm_repl_tty_read(evm_t * e);
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

#endif

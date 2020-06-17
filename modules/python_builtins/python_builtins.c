/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，支持主流 ROM > 50KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version : 1.0
**  Email   : scriptiot@aliyun.com
**  Website : https://github.com/scriptiot
**  Licence: MIT Licence
****************************************************************************/
#include "python_builtins.h"

evm_val_t python_abs(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    if( evm_is_integer(v) ) return evm_mk_number(fabs(evm_2_integer(v)));
    else return evm_mk_number(fabs(evm_2_double(v)));
}

evm_val_t python_all(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(!evm_is_list(v)) ARG_TYPE_ERR
    int len = evm_list_len(v);
    if(len == 0) return evm_mk_true();
    for (int i = 0; i < len; i++) {
        evm_val_t *val = evm_list_get(e, v, i);
        if (evm_is_integer(val) && evm_2_integer(val) == 0) return evm_mk_false();
        else if (evm_is_number(val) && fabs(evm_2_double(val) < 1e-15)) return evm_mk_false();
        else if (evm_is_string(val) && strlen(evm_2_string(val)) == 0) return evm_mk_false();
        else if (evm_is_boolean(val) && evm_2_boolean(val) == 0) return evm_mk_false();
        else if (evm_is_null(val)) return evm_mk_false();
    }
    return evm_mk_true();
}

evm_val_t python_any(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_list(v) ) ARG_TYPE_ERR
    int len = evm_list_len(v);
    if (len == 0) return evm_mk_false();
    int flag = 0;
    for (int i = 0; i < len; i++) {
        evm_val_t *val = evm_list_get(e, v, i);
        if (evm_is_integer(val) && evm_2_integer(val) == 0) flag++;
        else if (evm_is_number(val) && fabs(evm_2_double(val) < 1e-15)) flag++;
        else if (evm_is_string(val) && strlen(evm_2_string(v)) == 0) flag++;
        else if (evm_is_boolean(val) && evm_2_boolean(val) == 0) flag++;
        else if (evm_is_null(val)) flag++;
    }
    if (len == flag) return evm_mk_false();
    else return evm_mk_true();
}

evm_val_t python_ascii(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    return *evm_2_string(v);
}

evm_val_t python_bin(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(!evm_is_integer(v)) ARG_TYPE_ERR

    int val = evm_2_integer(v);
    int len = 0, flag = 0;

    if (val < 0) {
        val = -val;
        flag = 1;
    }

    if (val > 1 && log(val + 1) / log(2) < 1e-15) len++;

    while (val > 0) {
        val = val / 2;
        len++;
    }

    int start = 2, i = 0;
    if (flag == 1) {
        flag = 0;
        start = 3;
        evm_val_t *str = evm_heap_string_create(e, "-0b", len + start);

        int temp = -evm_2_integer(v), carry = 0;
        for (i = 0; i < len; i++) {
            if (i == 0 && temp % 2 == 0) flag = 1;
            if (flag == 1 && carry == 0) {
                carry = 1;
                evm_heap_string_set(str, "0", start + len - 1 - i, 1);
            } else {
                if (i == 0 && carry == 0) {
                    if (temp % 2 == 0) carry = 1;
                    evm_heap_string_set(str, "1", start + len - 1 - i, 1);
                } else if (carry == 1 && temp % 2 == 0) {
                    evm_heap_string_set(str, "0", start + len - 1 - i, 1);
                } else {
                    carry = 0;
                    evm_heap_string_set(str, "1", start + len - 1 - i, 1);
                }
            }
            temp /= 2;
        }
        return *str;
    } else {
        int temp = evm_2_integer(v);
        evm_val_t *str = evm_heap_string_create(e, "0b", len + start);
        for (i = len - 1; i >= 0; i--) {
            evm_heap_string_set(str, temp % 2 == 1 ? "1" : "0", start + i, 1);
            temp /= 2;
        }
        return *str;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_bool(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if (evm_is_integer(v) && evm_2_integer(v) == 0) return evm_mk_false();
    else if (evm_is_number(v) && fabs(evm_2_double(v) < 1e-15)) return evm_mk_false();
    else if (evm_is_string(v) && strlen(evm_2_string(v)) == 0) return evm_mk_false();
    else if (evm_is_boolean(v) && evm_2_boolean(v) == 0) return evm_mk_false();
    else if (evm_is_list(v) && evm_list_len(v) == 0) return evm_mk_false();
    else if (evm_is_null(v)) return evm_mk_false();
    else return evm_mk_true();
}

evm_val_t python_bytes(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if (argc > 1) ARG_LENGTH_ERR
    if (argc == 0) return *evm_buffer_create(e, 0);
    else if ( argc >= 1 ) {
        if( evm_is_number(v) ) {
            if (argc != 1) ARG_LENGTH_ERR
            else return *evm_buffer_create(e, evm_2_integer(v));
        } else if ( evm_is_string(v) ) {
            if (argc != 2) ARG_LENGTH_ERR
            else {
                evm_val_t *res = evm_buffer_create(e, strlen(evm_2_string(v)));
                return evm_buffer_set(res, (char *)evm_2_string(v), 0, strlen(evm_2_string(v)));
            }
        } else if ( evm_is_list(v) ) {
            if (argc != 1) ARG_LENGTH_ERR
            int len = evm_list_len(v);
            evm_val_t *res = evm_buffer_create(e, len);
            for(int i = 0; i < len; i++) {
                evm_buffer_set(res, (char *)evm_2_string(evm_list_get(e, v, i)), i, 1);
            }
            return *res;
        }
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_bytearray(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if( argc != 1 ) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    int size = evm_2_integer(v);
    return *evm_buffer_create(e, size);
}

evm_val_t python_dict(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    int val = evm_2_integer(v);
    return evm_mk_number(val);
}

evm_val_t python_float(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(evm_is_number(v)) return evm_mk_number(evm_2_double(v));
    else if (evm_is_string(v)) return evm_mk_number(atof(evm_2_string(v)));
    else ARG_TYPE_ERR
}

evm_val_t python_getattr(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc <= 1) ARG_LENGTH_ERR
    if( !evm_is_object(v) ) ARG_TYPE_ERR
    evm_val_t *res = evm_prop_get(e, v, evm_2_string(v + 1), 0);
    if (evm_is_undefined(res) && argc == 3) {
        return EVM_VAL_UNDEFINED;
    } else {
        return *res;
    }
}

evm_val_t python_hasattr(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 2) ARG_LENGTH_ERR
    if( !evm_is_object(v) ) ARG_TYPE_ERR
    evm_val_t *res = evm_prop_get(e, v, evm_2_string(v + 1), 0);
    if (res) return EVM_VAL_TRUE;
    else return EVM_VAL_FALSE;
}

evm_val_t python_hex(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    int val = evm_2_integer(v);

    if (val == 0) return *evm_heap_string_create(e, "0x0", 3);

    int temp = val, len = 2;

    if (temp < 0) {
        temp = -temp;
        len++;
    }

    while (temp) {
        temp = temp / 16;
        len++;
    }

    temp = val;
    if (temp < 0) {
        val = -val;
    }

    evm_val_t *str = evm_heap_string_create(e, "", len);

    if (temp < 0) {
        sprintf(evm_heap_string_addr(str), "-%#x", val);
    } else {
        sprintf(evm_heap_string_addr(str), "%#x", val);
    }
    
    return *str;
}

evm_val_t python_int(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(evm_is_number(v)) return evm_mk_number(evm_2_integer(v));
    else if(evm_is_string(v)) return evm_mk_number(atoi(evm_2_string(v)));
    else ARG_TYPE_ERR
}

evm_val_t python_len(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( evm_is_string(v) ) {
        return evm_mk_number(strlen(evm_2_string(v)));
    } else if( evm_is_buffer(v) ) {
        return evm_mk_number(evm_buffer_len(v));
    } else if( evm_is_list(v) ) {
        return evm_mk_number(evm_list_len(v));
    } else if ( evm_is_object(v) ) {
        return evm_mk_number(evm_prop_len(v));
    } else {
        ARG_TYPE_ERR
    }
}

evm_val_t python_list(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)e;
    (void)v;
    (void)p;
    (void)argc;
    if( argc != 0 && argc != 1 ) ARG_LENGTH_ERR;
    if( argc == 0 ){
        return *evm_list_create(e, GC_LIST, 0);
    } else if( argc == 1){
        if( !evm_is_number(v) ) ARG_TYPE_ERR;
        return *evm_list_create(e, GC_LIST, evm_2_integer(v));
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_max(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if( argc <= 0 ) ARG_LENGTH_ERR;
    
    if (evm_is_number(v)) {
        evm_val_t *max = v;
        for (int i = 1; i < argc; i++) {
            if (evm_is_number(v + i) && evm_2_double(max) < evm_2_double(v + i)) {
                max = v + i;
            }
        }
        return *max;
    }
    else if (evm_is_string(v)) {
        evm_val_t *max = v;
        for (int i = 1; i < argc; i++) {
            if (evm_is_string(v + i) && strcmp(evm_2_string(max), evm_2_string(v + i)) < 0) {
                max = v + i;
            }
        }
        return *max;
    }
    else if (evm_is_list(v)) {
        evm_val_t *o = evm_list_get(e, v, 0);
        int len = evm_list_len(v);
        if (evm_is_number(o)) {
            evm_val_t *max = evm_list_get(e, v, 0);
            for (int i = 1; i < len; i++) {
                o = evm_list_get(e, v, i);
                if (evm_is_number(o) && evm_2_double(max) < evm_2_double(o)) {
                    max = o;
                }
            }
            return *max;
        }
        else if (evm_is_string(o)) {
            evm_val_t *max = evm_list_get(e, v, 0);
            for (int i = 1; i < len; i++) {
                o = evm_list_get(e, v, i);
                if (evm_is_string(o) && strcmp(evm_2_string(max), evm_2_string(o)) < 0) {
                    max = o;
                }
            }
            return *max;
        }
        else return EVM_VAL_UNDEFINED;
    }
    else  ARG_TYPE_ERR;
}

evm_val_t python_min(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if( argc <= 0 ) ARG_LENGTH_ERR;
    
    if (evm_is_number(v)) {
        evm_val_t *min = v;
        for (int i = 1; i < argc; i++) {
            if (evm_is_number(v + i) && evm_2_double(min) > evm_2_double(v + i)) {
                min = v + i;
            }
        }
        return *min;
    }
    else if (evm_is_string(v)) {
        evm_val_t *min = v;
        for (int i = 1; i < argc; i++) {
            if (evm_is_string(v + i) && strcmp(evm_2_string(min), evm_2_string(v + i)) > 0) {
                min = v + i;
            }
        }
        return *min;
    }
    else if (evm_is_list(v)) {
        evm_val_t *o = evm_list_get(e, v, 0);
        int len = evm_list_len(v);
        if (evm_is_number(o)) {
            evm_val_t *min = evm_list_get(e, v, 0);
            for (int i = 1; i < len; i++) {
                o = evm_list_get(e, v, i);
                if (evm_is_number(o) && evm_2_double(min) > evm_2_double(o)) {
                    min = o;
                }
            }
            return *min;
        }
        else if (evm_is_string(o)) {
            evm_val_t *min = evm_list_get(e, v, 0);
            for (int i = 1; i < len; i++) {
                o = evm_list_get(e, v, i);
                if (evm_is_string(o) && strcmp(evm_2_string(min), evm_2_string(o)) > 0) {
                    min = o;
                }
            }
            return *min;
        }
        else return EVM_VAL_UNDEFINED;
    }
    else  ARG_TYPE_ERR;
}

evm_val_t python_oct(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if( !evm_is_number(v) ) ARG_TYPE_ERR
    int val = evm_2_integer(v);

    if (val == 0) return *evm_heap_string_create(e, "0o0", 3);

    int temp = val, len = 2;

    if (temp < 0) {
        temp = -temp;
        len++;
    }

    while (temp) {
        temp = temp / 8;
        len++;
    }

    temp = val;
    if (temp < 0) {
        val = -val;
    }

    evm_val_t *str = evm_heap_string_create(e, "", len);

    if (temp < 0) {
        sprintf(evm_heap_string_addr(str), "-0o%o", val);
    } else {
        sprintf(evm_heap_string_addr(str), "0o%o", val);
    }
    
    return *str;
}

evm_val_t python_pow(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc < 2) ARG_LENGTH_ERR

    if (evm_is_number(v) && evm_is_number(v + 1)) {
        double result = pow(evm_2_double(v), evm_2_double(v + 1));
        if(argc == 3 && evm_is_integer(v + 2)) {
            result = fmod(result, evm_2_double(v + 2));
        }
        return evm_mk_number(result);
    } else {
        ARG_TYPE_ERR
    }
}

evm_val_t python_repr(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    return *evm_2_string(v);
}

evm_val_t python_round(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc < 1) ARG_LENGTH_ERR
    if(!evm_is_number(v)) ARG_TYPE_ERR
    double result = round(evm_2_double(v));
    if (argc == 2 && evm_is_integer(v + 1)) {
        int div = 1, count = evm_2_integer(v + 1);
        while (count) {
            div *= 10;
            count--;
        }
        result = (int)(result * div + 0.5) / (div * 1.0);
    }
    return evm_mk_number(result);
}

evm_val_t python_setattr(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 3) ARG_LENGTH_ERR
    if( !evm_is_object(v) ) ARG_TYPE_ERR
    evm_val_t *res = evm_prop_get(e, v, evm_2_string(v + 1), 0);
    if (res) {
        evm_prop_set_value(e, v, evm_2_string(v + 1), *(v + 2));
    } else {
        evm_prop_append(e, v, (char *)evm_2_string(v + 1), (v + 2));
    }
    return EVM_VAL_NULL;
}

evm_val_t python_sum(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc < 1) ARG_LENGTH_ERR
    evm_val_t *o = v;
    if (!evm_is_list(o)) ARG_TYPE_ERR
    double result = 0;
    for (int i = 0; i < evm_list_len(o); i++) {
        if (evm_is_integer(evm_list_get(e, o, i))) {
            result += evm_2_integer(evm_list_get(e, o, i));
        } else if (evm_is_number(evm_list_get(e, o, i))) {
            result += evm_2_double(evm_list_get(e, o, i));
        }
    }
    if (argc == 2 && evm_is_number((v + 1))) {
        if (evm_is_integer((v + 1))) {
            result += evm_2_integer((v + 1));
        } else if (evm_is_number((v + 1))) {
            result += evm_2_double((v + 1));
        }
    }
    return evm_mk_number(result);
}

evm_val_t python_tuple(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(!evm_is_list(v)) ARG_TYPE_ERR
    int len = evm_list_len(v);
    evm_val_t * t = evm_list_create(e, GC_TUPLE, len);
    for(int i = 0; i < len; i++) evm_list_set(e, t, i, *evm_list_get(e, v, i));
    return *t;
}

evm_val_t python_type(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if(argc != 1) ARG_LENGTH_ERR
    if(evm_is_integer(v)) return evm_mk_foreign_string((intptr_t)"<class 'int'>");
    else if(evm_is_number(v)) return evm_mk_foreign_string((intptr_t)"<class 'float'>");
    else if(evm_is_string(v)) return evm_mk_foreign_string((intptr_t)"<class 'str'>");
    else if(evm_is_list(v)) return evm_mk_foreign_string((intptr_t)"<class 'list'>");
    else if(evm_is_object(v)) return evm_mk_foreign_string((intptr_t)"<class 'dict'>");
    else return EVM_VAL_UNDEFINED;
}

evm_val_t python_inner_tuple(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    evm_val_t * t = evm_list_create(e, GC_TUPLE, argc);
    for(int i = 0; i < argc; i++) evm_list_set(e, t, i, *(v + i));
    return *t;
}

evm_val_t python_inner_class(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    EVM_UNUSED(p);EVM_UNUSED(e);EVM_UNUSED(argc);
    evm_script_set_native(v, (evm_native_fn)evm_2_intptr(v + 1));
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_inner_init(evm_t * e, evm_val_t * p, int argc, evm_val_t * v)
{
    (void)p;
    if( evm_is_class(p) ){
        evm_val_t * obj = evm_object_create_by_class(e, GC_OBJECT, p);
        evm_val_t * parent = evm_get_parent(e, *p);

        evm_val_t * c_obj = obj;
        while(parent){
            evm_val_t * local_obj = evm_object_create_by_class(e, GC_OBJECT, parent);
            evm_set_parent(c_obj, local_obj);
            c_obj = local_obj;
            parent = evm_get_parent(e, *parent);
        }
        evm_run_callback(e, p, obj, NULL, 0);
        evm_val_t * init_fn = evm_prop_get(e, obj, "__init__", 0);
        if( init_fn ){
            *(++e->sp) = *obj;
            evm_val_t * argv = e->sp;
            for(int i = 0; i < argc; i++) {
                *(++e->sp) = *(v + i);
            }
            evm_run_callback(e, init_fn, obj, argv, argc + 1);
        }
        return *obj;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t python_super(evm_t * e, evm_val_t * p, int argc, evm_val_t * v){
    (void)e;
    (void)v;
    (void)p;
    if( argc != 2) ARG_LENGTH_ERR;
    if( !evm_is_class(v) || !evm_is_object(v + 1)) ARG_TYPE_ERR;
    return *evm_get_parent(e, *(v + 1));
}

int python_builtins(evm_t * e){
    evm_builtin_t natives[] = {
        {".tuple", evm_mk_native( (intptr_t)python_inner_tuple )},
        {".class", evm_mk_native( (intptr_t)python_inner_class )},
        {".init", evm_mk_native( (intptr_t)python_inner_init )},
        {"list", evm_mk_native( (intptr_t)python_list )},
        {"bytearray", evm_mk_native( (intptr_t)python_bytearray )},
        {"len", evm_mk_native( (intptr_t)python_len )},
        {"int", evm_mk_native( (intptr_t)python_int )},
        {"super", evm_mk_native( (intptr_t)python_super )},
        {"abs", evm_mk_native( (intptr_t)python_abs )},
        {"all", evm_mk_native( (intptr_t)python_all )},
        {"any", evm_mk_native( (intptr_t)python_any )},
        {"ascii", evm_mk_native( (intptr_t)python_ascii )},
        {"bin", evm_mk_native( (intptr_t)python_bin )},
        {"bool", evm_mk_native( (intptr_t)python_bool )},
        {"bytes", evm_mk_native( (intptr_t)python_bytes )},
        {"float", evm_mk_native( (intptr_t)python_float )},
        {"getattr", evm_mk_native( (intptr_t)python_getattr )},
        {"hasattr", evm_mk_native( (intptr_t)python_hasattr )},
        {"hex", evm_mk_native( (intptr_t)python_hex )},
        {"max", evm_mk_native( (intptr_t)python_max )},
        {"min", evm_mk_native( (intptr_t)python_min )},
        {"oct", evm_mk_native( (intptr_t)python_oct )},
        {"pow", evm_mk_native( (intptr_t)python_pow )},
        {"round", evm_mk_native( (intptr_t)python_round )},
        {"setattr", evm_mk_native( (intptr_t)python_setattr )},
        {"sum", evm_mk_native( (intptr_t)python_sum )},
        {"tuple", evm_mk_native( (intptr_t)python_tuple )},
        {"type", evm_mk_native( (intptr_t)python_type )},
        {NULL, NULL}
    };
    return evm_native_add(e, natives);
}
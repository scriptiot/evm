#ifdef CONFIG_EVM_MODULE_BUFFER
#include "evm_module.h"

evm_val_t *evm_module_buffer_class_instantiate(evm_t *e, evm_val_t *buffer, uint32_t size);

//new Buffer(array)
//new Buffer(buffer)
//new Buffer(size)
//new Buffer(str[, encoding])
static evm_val_t evm_module_buffer_class_new(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t *buf_obj = NULL;
    evm_val_t *buffer = NULL;
    uint32_t length = 0;

    if(evm_is_integer(v))
        length = (uint32_t)evm_2_integer(v);
    else if (evm_is_list(v))
        length = evm_list_len(v);
    else if (evm_is_buffer(v))
        length = evm_buffer_len(v);
    else if (evm_is_string(v)) {
        length = evm_string_len(v);
//        if (argc > 2 && evm_is_string(v + 1) && strcmp(evm_2_string(v + 1), "hex") == 0) {
//            uint32_t i, cnt = 0;
//            const char *p = evm_2_string(v);
//            uint32_t len = strlen(p);
//            buffer = evm_buffer_create(e, len);
//            char *buf = evm_buffer_addr(buffer);
//            while(*p != '\0' && *p) {
//                for (i = 0; i < len; i ++)
//                {
//                    if ((*p >= '0') && (*p <= '9'))
//                        buf[cnt] = *p - '0' + 0x30;
//                    if ((*p >= 'A') && (*p <= 'Z'))
//                        buf[cnt] = *p - 'A' + 0x41;
//                    if ((*p >= 'a') && (*p <= 'z'))
//                        buf[cnt] = *p - 'a' + 0x61;
//                    p++;
//                    cnt++;
//                }
//            }
//        }
    }

    buf_obj = evm_module_buffer_class_instantiate(e, buffer, length);

    if( buf_obj )
        return *buf_obj;
    return EVM_VAL_UNDEFINED;
}

//Buffer.byteLength(str, encoding)
//encoding: hex | bytes(default)
static evm_val_t evm_module_buffer_byteLength(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v)) {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t obj = evm_module_buffer_class_new(e, p, argc, v);

    evm_val_t length = evm_attr_get(e, &obj, "length");

    return evm_mk_number(evm_2_integer(&length));
}

//Buffer.concat(list)
static evm_val_t evm_module_buffer_concat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_list(v))
        return EVM_VAL_UNDEFINED;

    uint32_t length = evm_list_len(v);
    uint32_t buffer_length = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        buffer_length += evm_buffer_len(v + i);
    }

    evm_val_t *buf = evm_buffer_create(e, buffer_length);
    uint32_t i = 0;
    while (i < buffer_length)
    {
        evm_buffer_set(e, buf, (v + i), i, evm_buffer_len(v + i));
        i += evm_buffer_len(v + i);
    }

    return *buf;
}

//Buffer.from(array)
//Buffer.from(buffer)
//Buffer.from(string[,encoding])
//Buffer.from(arrayBuffer[, byteOffset[, length]])
static evm_val_t evm_module_buffer_from(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1)
        return EVM_VAL_UNDEFINED;

    uint32_t length = 0;
    evm_val_t *buf;

    if (evm_is_list(v))
    {
        length = evm_buffer_len(v);
        buf = evm_buffer_create(e, length);
        if (!buf)
            return EVM_VAL_UNDEFINED;
        for (uint32_t i = 0; i < length; i++)
        {
            evm_buffer_set(e, buf, evm_buffer_addr(evm_mk_buffer(v + i)), i, 1);
        }
    }
    else if (evm_is_buffer(v))
    {
        length = evm_buffer_len(v);
        buf = evm_buffer_create(e, length);
        if (!buf)
            return EVM_VAL_UNDEFINED;
        evm_buffer_set(e, buf, evm_buffer_addr(v), 0, length);
    }
    else if (evm_is_string(v))
    {
        length = evm_string_len(v);
        buf = evm_buffer_create(e, length);
        if (!buf)
            return EVM_VAL_UNDEFINED;
        evm_buffer_set(e, buf, evm_buffer_addr(v), 0, length);
    }
    else
    {
        return EVM_VAL_UNDEFINED;
    }
    return *buf;
}

//Buffer.isBuffer(obj)
static evm_val_t evm_module_buffer_isBuffer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1)
        return EVM_VAL_UNDEFINED;
    if (!evm_is_object(v))
        return EVM_VAL_FALSE;

    if (evm_is_buffer(v))
        return EVM_VAL_TRUE;

    return EVM_VAL_FALSE;
}

//buf.length
static evm_val_t evm_module_buffer_class_get_length(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    uint32_t length = 0;
    length = evm_buffer_len(v);
    return evm_mk_number(length);
}

//buf.compare(otherBuffer)
static evm_val_t evm_module_buffer_class_compare(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t p_len = evm_buffer_len(p);
    uint32_t v_len = evm_buffer_len(v);
    // return -1 if first lower than second
    // return 1 if second lower than first
    // return 0 if first equal second
    if (p_len == v_len) {
        for(uint32_t i = 0; i < p_len; i++) {
            if (*(p + i) < *(v + i)) return evm_mk_number(-1);
            else if (*(p + i) > *(v + i)) return evm_mk_number(1);
        }
        return evm_mk_number(0);
    } else if (p_len < v_len) { // length different
        return evm_mk_number(1); // return 1 if first less than second
    } else { // p_len > v_len
        return evm_mk_number(-1); // return -1 if first rather than second
    }

    return EVM_VAL_UNDEFINED;
}

//buf.copy(targetBuffer[, targetStart[, sourceStart[, sourceEnd]]])
static evm_val_t evm_module_buffer_class_copy(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    char *target = evm_buffer_addr(v);

    uint32_t source_start = 0;
    uint32_t source_end = evm_buffer_len(p);
    if (source_start > source_end)
        return EVM_VAL_UNDEFINED;
    uint32_t target_start = 0;
    if (argc > 2 && evm_is_integer(v + 1))
        target_start = evm_2_integer(v + 1);

    for(uint32_t i = source_start; i < source_end; i++) {
        evm_buffer_set(e, target, target + target_start, i, 0);
    }
}

//buf.equals(otherBuffer)
static evm_val_t evm_module_buffer_class_equals(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t result = evm_module_buffer_class_compare(e, p, argc, v);
    return evm_2_integer(result) == 0 ? EVM_VAL_TRUE : EVM_VAL_FALSE;
}

//buf.fill(value)
static evm_val_t evm_module_buffer_class_fill(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t value = evm_2_integer(v);
    evm_val_t *buf = evm_buffer_create(e, 1);
    memcpy(evm_buffer_addr(buf), &value, sizeof(value));

    uint32_t length = evm_buffer_len(v);
    for(uint32_t i = 0; i < length; i++) {
        evm_buffer_set(e, p, buf, i, 1);
    }
}

//buf.slice([start[, end]])
static evm_val_t evm_module_buffer_class_slice(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // 根据开始下标和结束下标获取某一段buffer
    uint32_t start = 0;
    uint32_t end = evm_buffer_len(p);
    if (argc > 1 && evm_is_integer(v)) {
        start = evm_2_integer(v);
    }
    if (argc > 2 && evm_is_integer(v + 1)) {
        end = evm_2_integer(v + 1);
    }
    if (start > end)
        return EVM_VAL_UNDEFINED;

    uint32_t length = end - start;

    evm_val_t *buf = evm_buffer_create(e, length);

    if (!buf)
        return EVM_VAL_UNDEFINED;

    char *b = evm_buffer_addr(p);
    for(uint32_t i = start; i < end; i++) {
        evm_buffer_set(e, buf, b + i, i, 1);
    }

    return *b;
}

//buf.toString([start[, end]])
static evm_val_t evm_module_buffer_class_toString(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    uint32_t start = 0;
    uint32_t end = evm_buffer_len(p);

    if (argc > 1 && evm_is_integer(v))
        start = evm_2_integer(v);

    if (argc > 2 && evm_is_integer(v +1))
        end = evm_2_integer(v + 1);

    if (start > end)
        return EVM_VAL_UNDEFINED;

    evm_val_t *result = evm_heap_string_create(e, "$result", end - start);
    evm_heap_string_set(e, result, evm_buffer_addr(p), start, end - start);
    return *result;
}

//buf.write(string[, offset[, length]])
static evm_val_t evm_module_buffer_class_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    uint32_t offset = 0;
    uint32_t length = evm_string_len(v);

    if (argc > 2 && evm_is_integer(v + 1))
        offset = evm_2_integer(v +1);

    if (argc > 3 && evm_is_integer(v + 2))
        length = evm_2_integer(v + 2);

    evm_buffer_set(e, p, v, offset, length);
    return evm_mk_number(length);
}

//buf.writeUInt8(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_integer(v) || !evm_is_integer(v + 1))
        return EVM_VAL_UNDEFINED;

    uint32_t value = evm_2_integer(v);
    uint32_t offset = 0;
    if (argc > 2 && evm_is_integer(v + 1))
        offset = evm_2_integer(v + 1);

    evm_val_t *buf = evm_buffer_create(e, 4);

    uint8_t b[4];
    b[0] = 0xff & value;
    b[1] = 0xff & (value >> 8);
    b[2] = 0xff & (value >> 16);
    b[3] = 0xff & (value >> 24);

    // unsigned int 8 bit
    evm_buffer_set(e, p, b, offset, 4);
    return evm_mk_number(4);
}

//buf.writeUInt16LE(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt16LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // unsigned int 16 bit
    return EVM_VAL_UNDEFINED;
}

//buf.writeUInt32LE(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt32LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//buf.readInt8(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//buf.readUInt8(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readUInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//buf.readUInt16LE(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readUInt16LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_buffer_getter(evm_t *e, evm_val_t *p, const char *name, int argc, evm_val_t *v)
{
    if (evm_is_string(p))
    {
        if (!strcmp(name, "length"))
        {
            return evm_mk_number(strlen(evm_2_string(p)));
        }
    }
    else if (evm_is_object(p))
    {
        evm_val_t *ret = evm_prop_get(e, p, name, 0);
        if (ret)
            return *ret;
        ret = evm_attr_get(e, p, name);
        if (ret)
            return *ret;
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t *evm_module_buffer_class_instantiate(evm_t *e, evm_val_t *buffer, uint32_t size)
{
    evm_val_t *obj;
    if (buffer)
        obj = buffer;
    else
        obj = evm_buffer_create(e, size);

    if (obj)
    {
//        evm_attr_append(e, obj, "length", evm_mk_number(size));
        evm_attr_append(e, obj, "compare", evm_mk_native((intptr_t)evm_module_buffer_class_compare));
        evm_attr_append(e, obj, "copy", evm_mk_native((intptr_t)evm_module_buffer_class_copy));
        evm_attr_append(e, obj, "equals", evm_mk_native((intptr_t)evm_module_buffer_class_equals));
        evm_attr_append(e, obj, "fill", evm_mk_native((intptr_t)evm_module_buffer_class_fill));
        evm_attr_append(e, obj, "slice", evm_mk_native((intptr_t)evm_module_buffer_class_slice));
        evm_attr_append(e, obj, "toString", evm_mk_native((intptr_t)evm_module_buffer_class_toString));
        evm_attr_append(e, obj, "write", evm_mk_native((intptr_t)evm_module_buffer_class_write));
        evm_attr_append(e, obj, "writeUInt8", evm_mk_native((intptr_t)evm_module_buffer_class_writeUInt8));
        evm_attr_append(e, obj, "writeUInt16LE", evm_mk_native((intptr_t)evm_module_buffer_class_writeUInt16LE));
        evm_attr_append(e, obj, "writeUInt32LE", evm_mk_native((intptr_t)evm_module_buffer_class_writeUInt32LE));
        evm_attr_append(e, obj, "readInt8", evm_mk_native((intptr_t)evm_module_buffer_class_readInt8));
        evm_attr_append(e, obj, "readUInt8", evm_mk_native((intptr_t)evm_module_buffer_class_readUInt8));
        evm_attr_append(e, obj, "readUInt16LE", evm_mk_native((intptr_t)evm_module_buffer_class_readUInt16LE));
    }
    return obj;
}

static evm_object_native_t _buffer_native = {
    .creator = evm_module_buffer_class_new,
    .getter = evm_module_buffer_getter,
};

evm_err_t evm_module_buffer(evm_t *e)
{
    evm_val_t *buffer_func = evm_native_function_create(e, &_buffer_native, 0);
    evm_builtin_t builtin[] = {
        {"Buffer", *buffer_func},
        {"byteLength", evm_mk_native((intptr_t)evm_module_buffer_byteLength)},
        {"concat", evm_mk_native((intptr_t)evm_module_buffer_concat)},
        {"from", evm_mk_native((intptr_t)evm_module_buffer_from)},
        {"isBuffer", evm_mk_native((intptr_t)evm_module_buffer_isBuffer)},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "buffer", builtin);
    evm_pop(e);
    return e->err;
}
#endif

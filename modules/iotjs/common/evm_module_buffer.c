#ifdef CONFIG_EVM_MODULE_BUFFER
#include "evm_module.h"

evm_val_t *evm_module_buffer_class_instantiate(evm_t *e, uint32_t size);

static int hex_2_oct(int h) {
    int re = 0;
    int k = 16;
    int n = 1;
    while (h != 0)
    {
        re += (h % 10) * n;
        h /= 10;
        n *= k;
    }
    return re;
}

//new Buffer(array)
//new Buffer(buffer)
//new Buffer(size)
//new Buffer(str[, encoding])
static evm_val_t evm_module_buffer_class_new(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(p);
    evm_val_t *buf_obj = NULL;
    uint32_t length = 0;

    if (evm_is_integer(v)) {
        length = (uint32_t)evm_2_integer(v);
        buf_obj = evm_module_buffer_class_instantiate(e, length);
        EVM_ASSERT(buf_obj);
    } else if (evm_is_list(v)) {
        length = evm_list_len(v);
        buf_obj = evm_module_buffer_class_instantiate(e, length);
        EVM_ASSERT(buf_obj);
        evm_val_t *prop;
        uint8_t *buffer = evm_buffer_addr(buf_obj);
        EVM_ASSERT(buffer);
        uint8_t b;
        for (uint32_t i = 0; i < length; i++) {
            prop = evm_list_get(e, v, i);
            if (evm_is_number(prop)) {
                b = (uint8_t)evm_2_integer(prop);
                buffer[i] = b;
            }
        }
    } else if (evm_is_buffer(v)) {
        length = evm_buffer_len(v);
        buf_obj = evm_module_buffer_class_instantiate(e, length);
        EVM_ASSERT(buf_obj);
        evm_buffer_set(e, buf_obj, evm_buffer_addr(v), 0, length);
    } else if (evm_is_string(v)) {
        if (argc > 1 && evm_is_string(v + 1) && !strcmp(evm_2_string(v + 1), "hex")) {
            length = 0;
            uint8_t *p = (uint8_t *)evm_2_string(v);
            char t[2] = {0};
            while (*p != '\0' && *p) {
                strncpy(t, (const char *)p, 2);
                if (atoi(t))
                    length++;
                p += 2;
            }

            buf_obj = evm_module_buffer_class_instantiate(e, length);
            uint8_t *buffer = evm_buffer_addr(buf_obj);
            EVM_ASSERT(buf_obj);

            char n;
            p = (uint8_t *)evm_2_string(v);
            uint32_t cnt = 0;
            while (*p != '\0' && *p) {
                strncpy(t, (const char *)p, 2);
                n = (char)hex_2_oct(atoi(t));
                buffer[cnt] = n;
                p += 2;
                cnt++;
            }
        } else {
            length = evm_string_len(v);
            buf_obj = evm_module_buffer_class_instantiate(e, length);
            EVM_ASSERT(buf_obj);
            memcpy(evm_buffer_addr(buf_obj), evm_2_string(v), length);
        }
    }

    if( buf_obj )
        return *buf_obj;
    return EVM_VAL_UNDEFINED;
}

//Buffer.byteLength(str, encoding)
//encoding: hex | bytes(default)
static evm_val_t evm_module_buffer_byteLength(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    EVM_UNUSED(p);
    EVM_UNUSED(argc);
    EVM_UNUSED(v);
    return evm_mk_number(evm_string_len(v));
}

//Buffer.concat(list)
static evm_val_t evm_module_buffer_concat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(p);
    if (argc < 1 || !evm_is_list(v))
        return EVM_VAL_UNDEFINED;

    uint32_t len = evm_list_len(v);
    uint32_t length = 0;
    uint32_t buffer_length = 0;
    evm_val_t *prop;
    for (uint32_t i = 0; i < len; i++) {
        prop = evm_list_get(e, v, i);
        if (evm_is_buffer(prop)) {
            buffer_length += evm_buffer_len(prop);
            length++;
        }
    }

    evm_val_t *buf = evm_module_buffer_class_instantiate(e, buffer_length);
    EVM_ASSERT(buf);
    uint32_t i = 0, cnt = 0;
    while (i < length) {
        prop = evm_list_get(e, v, i);
        evm_buffer_set(e, buf, evm_buffer_addr(prop), cnt, evm_buffer_len(prop));
        cnt += evm_buffer_len(prop);
        i++;
    }

    return *buf;
}

//Buffer.from(array)
//Buffer.from(buffer)
//Buffer.from(string[,encoding])
//Buffer.from(arrayBuffer[, byteOffset[, length]])
static evm_val_t evm_module_buffer_from(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_buffer_class_new(e, p, argc, v);
}

//Buffer.isBuffer(buffer)
static evm_val_t evm_module_buffer_isBuffer(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    EVM_UNUSED(p);
    if (argc < 1)
        return EVM_VAL_UNDEFINED;

    if (evm_is_buffer(v))
        return EVM_VAL_TRUE;

    return EVM_VAL_FALSE;
}

//buf.length
static evm_val_t evm_module_buffer_class_get_length(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);EVM_UNUSED(p);EVM_UNUSED(argc);
    return evm_mk_number(evm_buffer_len(v));
}

//buf.compare(otherBuffer)
static evm_val_t evm_module_buffer_class_compare(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t p_len = evm_buffer_len(p);
    uint32_t v_len = evm_buffer_len(v);
    // return -1 if first lower than second
    // return 1 if second lower than first
    // return 0 if first equal second
    uint8_t *p_addr = evm_buffer_addr(p);
    uint8_t *v_addr = evm_buffer_addr(v);
    if (p_len == v_len) {
        for(uint32_t i = 0; i < p_len; i++) {
            if (p_addr[i] < v_addr[i]) return evm_mk_number(-1);
            else if (p_addr[i] > v_addr[i]) return evm_mk_number(1);
        }
        return evm_mk_number(0);
    } else if (p_len < v_len) { // length different
        return evm_mk_number(-1); // return 1 if first less than second
    } else { // p_len > v_len
        return evm_mk_number(1); // return -1 if first rather than second
    }

    return EVM_VAL_UNDEFINED;
}

//buf.copy(targetBuffer[, targetStart[, sourceStart[, sourceEnd]]])
static evm_val_t evm_module_buffer_class_copy(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    uint8_t *source = evm_buffer_addr(p);
    uint8_t *target = evm_buffer_addr(v);
    uint32_t target_length = evm_buffer_len(v);
    uint32_t source_start = 0;
    uint32_t source_end = evm_buffer_len(p);

    uint32_t target_start = 0;
    if (argc >= 1 && evm_is_integer(v + 1))
        target_start = evm_2_integer(v + 1);
    if (argc >= 2 && evm_is_integer(v + 2))
        source_start = evm_2_integer(v + 2);
    if (argc >= 3 && evm_is_integer(v + 3))
        source_end = evm_2_integer(v + 3);

    if (source_start > source_end)
        return EVM_VAL_UNDEFINED;

    for(uint32_t i = target_start; i < target_length; i++) {
        if (source_start + i < source_end)
            memcpy(target + target_start + i, source + source_start + i, 1);
    }
    return evm_mk_number(source_end - source_start);
}

//buf.equals(otherBuffer)
static evm_val_t evm_module_buffer_class_equals(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_buffer(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t result = evm_module_buffer_class_compare(e, p, argc, v);
    return result == 0 ? EVM_VAL_TRUE : EVM_VAL_FALSE;
}

//buf.fill(value)
static evm_val_t evm_module_buffer_class_fill(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t value = evm_2_integer(v);
    uint8_t *buffer = evm_buffer_addr(p);
    EVM_ASSERT(buffer);

    uint32_t length = evm_buffer_len(p);
    for(uint32_t i = 0; i < length; i++) {
        buffer[i] = value;
    }
    return *p;
}

//buf.slice([start[, end]])
static evm_val_t evm_module_buffer_class_slice(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    // 根据开始下标和结束下标获取某一段buffer
    uint32_t start = 0;
    uint32_t end = evm_buffer_len(p);
    if (argc >= 1 && evm_is_integer(v)) {
        start = evm_2_integer(v);
    }
    if (argc >= 2 && evm_is_integer(v + 1)) {
        end = evm_2_integer(v + 1);
    }
    if (start > end)
        return EVM_VAL_UNDEFINED;

    uint32_t length = end - start;

    evm_val_t *buf = evm_module_buffer_class_instantiate(e, length);
    if (!buf)
        return EVM_VAL_UNDEFINED;

    char *b = (char *)evm_buffer_addr(p);
    uint32_t cnt = 0;
    for(uint32_t i = start; i < end; i++) {
        evm_buffer_set(e, buf, (uint8_t *)(b + i), cnt, 1);
        cnt++;
    }

    return *buf;
}

//buf.toString([start[, end]])
static evm_val_t evm_module_buffer_class_toString(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    uint32_t start = 0;
    uint32_t end = evm_buffer_len(p);

    if (argc > 0 && evm_is_integer(v))
        start = evm_2_integer(v);

    if (argc > 1 && evm_is_integer(v + 1))
        end = evm_2_integer(v + 1);

    if (start > end)
        return EVM_VAL_UNDEFINED;

    evm_val_t *result = evm_heap_string_create(e, "", end - start);
    EVM_ASSERT(result);
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

    if (argc > 1 && evm_is_integer(v + 1))
        offset = evm_2_integer(v + 1);

    if (argc > 2 && evm_is_integer(v + 2))
        length = evm_2_integer(v + 2);

    evm_buffer_set(e, p, (uint8_t *)evm_2_string(v), offset, length);
    return evm_mk_number(length);
}

//buf.writeUInt8(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t offset = 0;
    if (argc > 1 && evm_is_integer(v + 1)) {
        offset = evm_2_integer(v + 1);
    }

    if (offset > evm_buffer_len(p)) {
        evm_set_err(e, ec_type, "buffer subscript index overflow");
        return NULL;
    }

    uint8_t value = evm_2_integer(v);
    if (value < 0 || value > 255) {
        evm_set_err(e, ec_type, "value must between 0 and 255");
        return NULL;
    }

    uint8_t *buf = evm_buffer_addr(p);

    buf[offset] = evm_2_integer(v);
    return evm_mk_number(offset + 1);
}

//buf.writeUInt16LE(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt16LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t offset = 0;
    if (argc > 1 && evm_is_integer(v + 1))
        offset = evm_2_integer(v + 1);

    if (offset + 2 > evm_buffer_len(p)) {
        evm_set_err(e, ec_type, "buffer subscript index overflow");
        return NULL;
    }

    uint8_t *buf = evm_buffer_addr(p);
    uint32_t value = (uint32_t)evm_2_integer(v);
//    buf[offset] = evm_2_integer(v);
    memcpy(buf + offset, &value, sizeof(uint16_t));
    return evm_mk_number(offset + 1);
}

//buf.writeUInt32LE(value, offset[, noAssert])
static evm_val_t evm_module_buffer_class_writeUInt32LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    uint32_t offset = 0;
    if (argc > 1 && evm_is_integer(v + 1))
        offset = evm_2_integer(v + 1);

    if (offset + 4 > evm_buffer_len(p)) {
        evm_set_err(e, ec_type, "buffer subscript index overflow");
        return NULL;
    }

    uint8_t *buf = evm_buffer_addr(p);
//    buf[offset] = evm_2_integer(v);
    uint32_t value = (uint32_t)evm_2_integer(v);
    memcpy(buf + offset, &value, sizeof(uint32_t));
    return evm_mk_number(offset + 1);
}

//buf.readInt8(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    uint32_t offset = 0;
    if (argc > 0 && evm_is_integer(v))
        offset = evm_2_integer(v);

    uint8_t *buf = evm_buffer_addr(p);
    int8_t value = (int8_t)buf[offset];

    return evm_mk_number(value);
}

//buf.readUInt8(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readUInt8(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    uint32_t offset = 0;
    if (argc > 0 && evm_is_integer(v))
        offset = evm_2_integer(v);

    uint8_t *buf = evm_buffer_addr(p);
    uint8_t value = (uint8_t)buf[offset];

    return evm_mk_number(value);
}

//buf.readUInt16LE(offset[, noAssert])
static evm_val_t evm_module_buffer_class_readUInt16LE(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    EVM_UNUSED(e);
    uint32_t offset = 0;
    if (argc > 0 && evm_is_integer(v))
        offset = evm_2_integer(v);

    uint8_t *buf = evm_buffer_addr(p);
    uint16_t value = 0;
    memcpy(&value, buf, sizeof(uint16_t));

    return evm_mk_number(value);
}

evm_val_t *evm_module_buffer_class_instantiate(evm_t *e, uint32_t size)
{
    evm_val_t *obj = evm_buffer_create(e, size);

    if (obj)
    {
        evm_err_t err = evm_attr_create(e, obj, 14);
        if( err == ec_ok ) {
            evm_attr_append(e, obj, "length", evm_mk_number(size));
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
    }
    return obj;
}

static evm_object_native_t _buffer_native = {
    .creator = evm_module_buffer_class_new,
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

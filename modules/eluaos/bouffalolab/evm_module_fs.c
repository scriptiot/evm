#include "evm.h"
#include <errno.h>
#include <vfs.h>
#include <aos/kernel.h>
#include <bl_romfs.h>

#define O_RDONLY 0
#define O_RDWR 2
#define O_CREAT 0x0200
#define O_WRONLY 1
#define O_APPEND 02000

//stats.isDirectory()
static evm_val_t evm_module_fs_stats_isDirectory(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct stat *st = (struct stat *)evm_object_get_ext_data(p);
    if (!st)
        return EVM_VAL_UNDEFINED;

    if (S_ISDIR(st->st_mode))
        return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
}

//stats.isFile()
static evm_val_t evm_module_fs_stats_isFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct stat *st = (struct stat *)evm_object_get_ext_data(p);
    if (!st)
        return EVM_VAL_UNDEFINED;

    if (S_ISREG(st->st_mode))
        return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
}

//fs.close(fd)
static evm_val_t evm_module_fs_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0)
        return EVM_VAL_UNDEFINED;

    if (!evm_is_integer(v) || evm_2_integer(v) == -1)
        return EVM_VAL_UNDEFINED;
    aos_close(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}

//fs.exists
static evm_val_t evm_module_fs_exists(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
        return EVM_VAL_FALSE;
    if (aos_access(evm_2_string(v), 0) == 0)
    {
        return EVM_VAL_TRUE;
    }
    return EVM_VAL_FALSE;
}

//fs.fstat(fd)
static evm_val_t evm_module_fs_fstat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_integer(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 2, 0);
    if (!obj)
        return EVM_VAL_UNDEFINED;

    evm_prop_append(e, obj, "isDirectory", evm_mk_native((intptr_t)evm_module_fs_stats_isDirectory));
    evm_prop_append(e, obj, "isFile", evm_mk_native((intptr_t)evm_module_fs_stats_isFile));
    struct stat *st = evm_malloc(sizeof(struct stat));
    // aos_fstat(evm_2_integer(v), st); // 该函数未实现
    evm_object_set_ext_data(obj, (intptr_t)st);
    return *obj;
}

//fs.mkdir(path[, mode])
static evm_val_t evm_module_fs_mkdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    aos_mkdir(evm_2_string(v));
    return EVM_VAL_UNDEFINED;
}

//fs.open(path, flags[, mode])
static evm_val_t evm_module_fs_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_string(v) || !evm_is_string(v + 1))
        return EVM_VAL_UNDEFINED;

    const char *flag = evm_2_string(v + 1);
    int mode = O_RDONLY;
    if (!strcmp(flag, "r"))
    {
        mode = O_RDONLY;
    }
    else if (!strcmp(flag, "rs"))
    {
        mode = O_RDONLY;
    }
    else if (!strcmp(flag, "r+"))
    {
        mode = O_RDWR;
    }
    else if (!strcmp(flag, "w"))
    {
        mode = O_CREAT | O_WRONLY;
    }
    else if (!strcmp(flag, "wx") || !strcmp(flag, "xw"))
    {
        mode = O_WRONLY;
    }
    else if (!strcmp(flag, "w+"))
    {
        mode = O_CREAT | O_RDWR;
    }
    else if (!strcmp(flag, "wx+") || !strcmp(flag, "xw+"))
    {
        mode = O_RDWR;
    }
    else if (!strcmp(flag, "a"))
    {
        mode = O_CREAT | O_APPEND;
    }
    else if (!strcmp(flag, "ax"))
    {
        mode = O_APPEND;
    }
    else if (!strcmp(flag, "a+"))
    {
        mode = O_CREAT | O_APPEND | O_RDONLY;
    }
    else if (!strcmp(flag, "ax+") || !strcmp(flag, "xa+"))
    {
        mode = O_APPEND | O_RDONLY;
    }
    return evm_mk_number(aos_open(evm_2_string(v), mode));
}

//fs.read(fd, buffer, offset, length, position)
static evm_val_t evm_module_fs_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int fd;
    void *buffer;
    size_t offset;
    size_t length;
    size_t position;

    if (argc < 5)
        return EVM_VAL_UNDEFINED;

    if (!evm_is_integer(v) || !evm_is_buffer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_integer(v + 4))
        return EVM_VAL_UNDEFINED;

    fd = evm_2_integer(v);
    if (fd == -1)
        return evm_mk_number(0);

    buffer = evm_buffer_addr(v + 1);
    offset = evm_2_integer(v + 2);
    length = evm_2_integer(v + 3);
    position = evm_2_integer(v + 4);

    return evm_mk_number(aos_read(fd, buffer + offset, length));
}

//fs.readFile(path)
static evm_val_t evm_module_fs_readFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;

    struct stat st;
    if (stat(evm_2_string(v), &st) < 0)
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *buf_obj = evm_buffer_create(e, st.st_size);
    if (!buf_obj)
        return EVM_VAL_UNDEFINED;

    int fd = aos_open(evm_2_string(v), 0);
    if (fd == -1)
        return EVM_VAL_UNDEFINED;

    aos_read(fd, evm_buffer_addr(buf_obj), st.st_size);
    aos_close(fd);
    return *buf_obj;
}

//fs.rename(oldPath, newPath)
static evm_val_t evm_module_fs_rename(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_string(v) || !evm_is_string(v + 1))
        return EVM_VAL_UNDEFINED;
    aos_rename(evm_2_string(v), evm_2_string(v + 1));
    return EVM_VAL_UNDEFINED;
}

//fs.rmdir(path)
static evm_val_t evm_module_fs_rmdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
        return EVM_VAL_UNDEFINED;
    aos_rmdir(evm_2_string(v));
    return EVM_VAL_UNDEFINED;
}

//fs.stat(path)
static evm_val_t evm_module_fs_stat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 2, 0);
    if (!obj)
        return EVM_VAL_UNDEFINED;

    evm_prop_append(e, obj, "isDirectory", evm_mk_native((intptr_t)evm_module_fs_stats_isDirectory));
    evm_prop_append(e, obj, "isFile", evm_mk_native((intptr_t)evm_module_fs_stats_isFile));

    struct stat *st = evm_malloc(sizeof(struct stat));
    if (!st)
        return EVM_VAL_UNDEFINED;
    aos_stat(evm_2_string(v), st);
    evm_object_set_ext_data(obj, (intptr_t)st);
    return *obj;
}

//fs.unlink(path)
static evm_val_t evm_module_fs_unlink(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc == 0 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }
    aos_unlink(evm_2_string(v));
    return EVM_VAL_UNDEFINED;
}

//fs.write(fd, buffer, offset, length[, position])
static evm_val_t evm_module_fs_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int fd;
    void *buffer;
    size_t offset;
    size_t length;

    if (argc < 5)
        return EVM_VAL_UNDEFINED;

    if (!evm_is_integer(v) || !evm_is_buffer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_integer(v + 4))
        return EVM_VAL_UNDEFINED;

    fd = evm_2_integer(v);
    if (fd == -1)
        return evm_mk_number(0);

    buffer = evm_buffer_addr(v + 1);
    offset = evm_2_integer(v + 2);
    length = evm_2_integer(v + 3);

    return evm_mk_number(aos_write(fd, buffer + offset, length));
}

//fs.writeFile(path, data)
static evm_val_t evm_module_fs_writeFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 2 || !evm_is_string(v) || !(evm_is_buffer(v + 1) || evm_is_string(v + 1)))
        return EVM_VAL_UNDEFINED;

    int fd = aos_open(evm_2_string(v), O_CREAT | O_WRONLY);
    if (fd == -1)
        return EVM_VAL_UNDEFINED;

    if (evm_is_buffer(v + 1))
        aos_write(fd, evm_buffer_addr(v + 1), evm_buffer_len(v + 1));
    else
    {
        aos_write(fd, evm_2_string(v + 1), evm_string_len(v + 1));
    }
    aos_close(fd);
    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_fs(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"close", evm_mk_native((intptr_t)evm_module_fs_close)},
        {"exists", evm_mk_native((intptr_t)evm_module_fs_exists)},
        {"fstat", evm_mk_native((intptr_t)evm_module_fs_fstat)},
        {"mkdir", evm_mk_native((intptr_t)evm_module_fs_mkdir)},
        {"open", evm_mk_native((intptr_t)evm_module_fs_open)},
        {"read", evm_mk_native((intptr_t)evm_module_fs_read)},
        {"readFile", evm_mk_native((intptr_t)evm_module_fs_readFile)},
        {"rename", evm_mk_native((intptr_t)evm_module_fs_rename)},
        {"rmdir", evm_mk_native((intptr_t)evm_module_fs_rmdir)},
        {"stat", evm_mk_native((intptr_t)evm_module_fs_stat)},
        {"unlink", evm_mk_native((intptr_t)evm_module_fs_unlink)},
        {"write", evm_mk_native((intptr_t)evm_module_fs_write)},
        {"writeFile", evm_mk_native((intptr_t)evm_module_fs_writeFile)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "fs", builtin);
    return e->err;
}

#ifdef CONFIG_EVM_MODULE_FS
#include "evm_module.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <rtthread.h>
#include <dfs_fs.h>
#include <errno.h>

//stats.isDirectory()
static evm_val_t evm_module_fs_stats_isDirectory(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct stat *st = (struct stat *)evm_object_get_ext_data(p);
    if( !st )
        return EVM_VAL_UNDEFINED;

    if( S_ISDIR(st->st_mode) )
        return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
}

//stats.isFile()
static evm_val_t evm_module_fs_stats_isFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct stat *st = (struct stat *)evm_object_get_ext_data(p);
    if( !st )
        return EVM_VAL_UNDEFINED;

    if( S_ISREG(st->st_mode) )
        return EVM_VAL_TRUE;
    return EVM_VAL_FALSE;
}

//fs.close(fd)
static evm_val_t evm_module_fs_close(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 )
        return EVM_VAL_UNDEFINED;

    if( !evm_is_integer(v) || evm_2_integer(v) == -1 )
        return EVM_VAL_UNDEFINED;
    close(evm_2_integer(v));
    return EVM_VAL_UNDEFINED;
}

//fs.closeSync(fd)
static evm_val_t evm_module_fs_closeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 )
        return EVM_VAL_UNDEFINED;

    if( !evm_is_integer(v) || evm_2_integer(v) == -1 )
        return EVM_VAL_UNDEFINED;
    close(evm_2_integer(v));

    return EVM_VAL_UNDEFINED;
}

//fs.createReadStream
static evm_val_t evm_module_fs_createReadStream(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.createWriteStream
static evm_val_t evm_module_fs_createWriteStream(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.existsSync
static evm_val_t evm_module_fs_existsSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_string(v) )
        return EVM_VAL_UNDEFINED;
    struct stat st;
    if( stat(evm_2_string(v), &st) != -1 ) {
        return EVM_VAL_TRUE;
    }
    return EVM_VAL_FALSE;
}

//fs.exists
static evm_val_t evm_module_fs_exists(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t exists = evm_module_fs_existsSync(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_run_callback(e, v + 1, &e->scope, &exists, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.fstatSync(fd)
static evm_val_t evm_module_fs_fstatSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_integer(v) )
        return EVM_VAL_UNDEFINED;
    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 2, 0);
    if( !obj )
        return EVM_VAL_UNDEFINED;
    evm_prop_append(e, obj, "isDirectory", evm_mk_native((intptr_t)evm_module_fs_stats_isDirectory));
    evm_prop_append(e, obj, "isFile", evm_mk_native((intptr_t)evm_module_fs_stats_isFile));
    struct stat *st = evm_malloc(sizeof(struct stat));
    fstat(evm_2_integer(v), st);
    evm_object_set_ext_data(obj, (intptr_t)st);
    return *obj;
}

//fs.fstat(fd, callback)
static evm_val_t evm_module_fs_fstat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t obj = evm_module_fs_fstatSync(e, p, argc, v);
    if( obj == EVM_VAL_UNDEFINED )
        return EVM_VAL_UNDEFINED;
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args[2];
        args[0] = EVM_VAL_NULL;
        args[1] = obj;
        evm_run_callback(e, v + 1, &e->scope, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.mkdir(path[, mode], callback)
static evm_val_t evm_module_fs_mkdirSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    __mode_t mode = 777;
    if( argc == 0 || !evm_is_string(v) )
        return EVM_VAL_UNDEFINED;
    if( argc > 1 && evm_is_integer(v + 1) )
        mode = (__mode_t)evm_2_integer(v + 1);
    mkdir(evm_2_string(v), mode);
    return EVM_VAL_UNDEFINED;
}

//fs.mkdir
static evm_val_t evm_module_fs_mkdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_fs_mkdirSync(e, p, argc, v);
    evm_val_t args = evm_mk_number(errno);
    if( argc > 2 && evm_is_script(v + 2) ) {
        evm_run_callback(e, v + 2, &e->scope, &args, 1);
    } else if( argc > 1 && evm_is_script(v + 1) ){
        evm_run_callback(e, v + 1, &e->scope, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.openSync(path, flags[, mode])
static evm_val_t evm_module_fs_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc < 2 || !evm_is_string(v) || !evm_is_string(v + 1) )
        return EVM_VAL_UNDEFINED;
    const char *flag = evm_2_string(v + 1);
    int mode;
    if( !strcmp(flag, "r") ) {
        mode = O_RDONLY;
    } else if( !strcmp(flag, "rs") ) {
        mode = O_RDONLY;
    } else if( !strcmp(flag, "r+") ) {
        mode = O_RDWR;
    } else if( !strcmp(flag, "w") ) {
        mode = O_CREAT | O_WRONLY;
    } else if( !strcmp(flag, "wx") || !strcmp(flag, "xw") ) {
        mode = O_WRONLY;
    } else if( !strcmp(flag, "w+") ) {
        mode = O_CREAT | O_RDWR;
    } else if( !strcmp(flag, "wx+") || !strcmp(flag, "xw+") ) {
        mode = O_RDWR;
    } else if( !strcmp(flag, "a") ) {
        mode = O_CREAT | O_APPEND;
    } else if( !strcmp(flag, "ax") ) {
        mode = O_APPEND;
    } else if( !strcmp(flag, "a+") ) {
        mode = O_CREAT | O_APPEND | O_RDONLY;
    } else if( !strcmp(flag, "ax+") || !strcmp(flag, "xa+") ) {
        mode = O_APPEND | O_RDONLY;
    }
    return evm_mk_number(open(evm_2_string(v), mode));
}

//fs.open(path, flags[, mode], callback)
static evm_val_t evm_module_fs_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_fs_openSync(e, p, argc, v);
    if(argc > 2 && evm_is_script(v + 2) ) {
        evm_val_t args[2];
        args[0] = evm_mk_number(errno);
        args[1] = ret;
        evm_run_callback(e, v + 2, &e->scope, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.readSync(fd, buffer, offset, length, position)
static evm_val_t evm_module_fs_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int fd;
    void *buffer;
    size_t offset;
    size_t length;
    size_t position;

    if( argc < 5 )
        return EVM_VAL_UNDEFINED;

    if( !evm_is_integer(v) || !evm_is_buffer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_integer(v + 4) )
        return EVM_VAL_UNDEFINED;

    fd = evm_2_integer(v);
    if( fd == -1 )
        return evm_mk_number(0);

    buffer = evm_buffer_addr(v + 1);
    offset = evm_2_integer(v + 2);
    length = evm_2_integer(v + 3);
    position = evm_2_integer(v + 4);

    return evm_mk_number( read(fd, buffer + offset, length) );
}

//fs.read(fd, buffer, offset, length, position, callback)
static evm_val_t evm_module_fs_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_fs_readSync(e, p, argc, v);
    if(argc > 4 && evm_is_script(v + 4) ) {
        evm_val_t args[3];
        args[0] = evm_mk_number(errno);
        args[1] = ret;
        args[2] = *(v + 1);
        evm_run_callback(e, v + 2, &e->scope, args, 3);
    }
}

//fs.readdir
static evm_val_t evm_module_fs_readdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.readdirSync
static evm_val_t evm_module_fs_readdirSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.readFileSync(path)
static evm_val_t evm_module_fs_readFileSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_string(v) )
        return EVM_VAL_UNDEFINED;

    struct stat st;
    if(stat(evm_2_string(v), &st) < 0){
       return EVM_VAL_UNDEFINED;
    }

    evm_val_t *buf_obj = evm_buffer_create(e, st.st_size);
    if( !buf_obj )
        return EVM_VAL_UNDEFINED;

    int fd = open(evm_2_string(v), O_RDONLY);
    if( fd == -1 )
        return EVM_VAL_UNDEFINED;

    read(fd, evm_buffer_addr(buf_obj), st.st_size);
    close(fd);
    return *buf_obj;
}

//fs.readFile(path, callback)
static evm_val_t evm_module_fs_readFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_fs_readFileSync(e, p, argc, v);
    if( ret == EVM_VAL_UNDEFINED )
        return ret;
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args[2];
        args[0] = evm_mk_number(errno);
        args[1] = ret;
        evm_run_callback(e, v + 1, &e->scope, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.renameSync(oldPath, newPath)
static evm_val_t evm_module_fs_renameSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc < 2 || !evm_is_string(v) || !evm_is_string(v + 1) )
        return EVM_VAL_UNDEFINED;
    rename(evm_2_string(v), evm_2_string(v + 1));
    return EVM_VAL_UNDEFINED;
}

//fs.rename
static evm_val_t evm_module_fs_rename(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_fs_renameSync(e, p, argc, v);
    if( argc > 2 && evm_is_script(v + 2) ) {
        evm_val_t args;
        args = evm_mk_number(errno);
        evm_run_callback(e, v + 2, &e->scope, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.rmdirSync(path)
static evm_val_t evm_module_fs_rmdirSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_string(v) )
        return EVM_VAL_UNDEFINED;
    rmdir(evm_2_string(v));
    return EVM_VAL_UNDEFINED;
}

//fs.rmdir(path, callback)
static evm_val_t evm_module_fs_rmdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_fs_rmdirSync(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args;
        args = evm_mk_number(errno);
        evm_run_callback(e, v + 1, &e->scope, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.statSync(path)
static evm_val_t evm_module_fs_statSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_string(v) ) {
        return EVM_VAL_UNDEFINED;
    }

    evm_val_t *obj = evm_object_create(e, GC_OBJECT, 2, 0);
    if( !obj )
        return EVM_VAL_UNDEFINED;

    evm_prop_append(e, obj, "isDirectory", evm_mk_native((intptr_t)evm_module_fs_stats_isDirectory));
    evm_prop_append(e, obj, "isFile", evm_mk_native((intptr_t)evm_module_fs_stats_isFile));

    struct stat *st = evm_malloc(sizeof(struct stat));
    if( !st )
        return EVM_VAL_UNDEFINED;
    stat(evm_2_string(v), st);
    evm_object_set_ext_data(obj, (intptr_t)st);
    return *obj;
}

//fs.stat(path, callback)
static evm_val_t evm_module_fs_stat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t obj = evm_module_fs_statSync(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args[2];
        args[0] = evm_mk_null();
        args[1] = obj;
        evm_run_callback(e, v + 1, &e->scope, args, 2);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.unlinkSync(path)
static evm_val_t evm_module_fs_unlinkSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc == 0 || !evm_is_string(v) ) {
        return EVM_VAL_UNDEFINED;
    }
    unlink(evm_2_string(v));
    return EVM_VAL_UNDEFINED;
}

//fs.unlink(path, callback)
static evm_val_t evm_module_fs_unlink(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_fs_unlinkSync(e, p, argc, v);
    if( argc > 1 && evm_is_script(v + 1) ) {
        evm_val_t args;
        args = evm_mk_number(errno);
        evm_run_callback(e, v + 1, &e->scope, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.writeSync(fd, buffer, offset, length[, position])
static evm_val_t evm_module_fs_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    int fd;
    void *buffer;
    size_t offset;
    size_t length;

    if( argc < 5 )
        return EVM_VAL_UNDEFINED;

    if( !evm_is_integer(v) || !evm_is_buffer(v + 1) || !evm_is_integer(v + 2) || !evm_is_integer(v + 3) || !evm_is_integer(v + 4) )
        return EVM_VAL_UNDEFINED;

    fd = evm_2_integer(v);
    if( fd == -1 )
        return evm_mk_number(0);

    buffer = evm_buffer_addr(v + 1);
    offset = evm_2_integer(v + 2);
    length = evm_2_integer(v + 3);

    return evm_mk_number( write(fd, buffer + offset, length) );
}

//fs.write(fd, buffer, offset, length[, position], callback)
static evm_val_t evm_module_fs_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_val_t ret = evm_module_fs_writeSync(e, p, argc, v);
    if(argc > 4 && evm_is_script(v + 4) ) {
        evm_val_t args[3];
        args[0] = evm_mk_number(errno);
        args[1] = ret;
        args[2] = *(v + 1);
        evm_run_callback(e, v + 2, &e->scope, args, 3);
    }
    return EVM_VAL_UNDEFINED;
}

//fs.writeFileSync(path, data)
static evm_val_t evm_module_fs_writeFileSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc < 2 || !evm_is_string(v) || !(evm_is_buffer(v + 1) || evm_is_string(v + 1) ) )
        return EVM_VAL_UNDEFINED;

    int fd = open(evm_2_string(v), O_CREAT | O_WRONLY);
    if( fd == -1 )
        return EVM_VAL_UNDEFINED;

    if( evm_is_buffer(v + 1) )
        write(fd, evm_buffer_addr(v + 1), evm_buffer_len(v + 1));
    else {
        write(fd, evm_2_string(v + 1), evm_string_len(v + 1));
    }
    close(fd);
    return EVM_VAL_UNDEFINED;
}

//fs.writeFile(path, data, callback)
static evm_val_t evm_module_fs_writeFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    evm_module_fs_writeFileSync(e, p, argc, v);
    if( argc > 2 && evm_is_script(v + 2) ) {
        evm_val_t args;
        args = evm_mk_number(errno);
        evm_run_callback(e, v + 1, &e->scope, &args, 1);
    }
    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_fs(evm_t *e) {
	evm_builtin_t builtin[] = {
		{"close", evm_mk_native((intptr_t)evm_module_fs_close)},
		{"closeSync", evm_mk_native((intptr_t)evm_module_fs_closeSync)},
        {"createReadStream", evm_mk_native((intptr_t)evm_module_fs_createReadStream)},
		{"createWriteStream", evm_mk_native((intptr_t)evm_module_fs_createWriteStream)},
        {"exists", evm_mk_native((intptr_t)evm_module_fs_exists)},
		{"existsSync", evm_mk_native((intptr_t)evm_module_fs_existsSync)},
        {"fstat", evm_mk_native((intptr_t)evm_module_fs_fstat)},
		{"fstatSync", evm_mk_native((intptr_t)evm_module_fs_fstatSync)},
        {"mkdir", evm_mk_native((intptr_t)evm_module_fs_mkdir)},
		{"mkdirSync", evm_mk_native((intptr_t)evm_module_fs_mkdirSync)},
        {"open", evm_mk_native((intptr_t)evm_module_fs_open)},
		{"openSync", evm_mk_native((intptr_t)evm_module_fs_openSync)},
        {"read", evm_mk_native((intptr_t)evm_module_fs_read)},
		{"readSync", evm_mk_native((intptr_t)evm_module_fs_readSync)},
        {"readdir", evm_mk_native((intptr_t)evm_module_fs_readdir)},
		{"readdirSync", evm_mk_native((intptr_t)evm_module_fs_readdirSync)},
        {"readFile", evm_mk_native((intptr_t)evm_module_fs_readFile)},
		{"readFileSync", evm_mk_native((intptr_t)evm_module_fs_readFileSync)},
        {"rename", evm_mk_native((intptr_t)evm_module_fs_rename)},
		{"renameSync", evm_mk_native((intptr_t)evm_module_fs_renameSync)},
        {"rmdir", evm_mk_native((intptr_t)evm_module_fs_rmdir)},
		{"rmdirSync", evm_mk_native((intptr_t)evm_module_fs_rmdirSync)},
        {"stat", evm_mk_native((intptr_t)evm_module_fs_stat)},
		{"statSync", evm_mk_native((intptr_t)evm_module_fs_statSync)},
        {"unlink", evm_mk_native((intptr_t)evm_module_fs_unlink)},
		{"unlinkSync", evm_mk_native((intptr_t)evm_module_fs_unlinkSync)},
        {"write", evm_mk_native((intptr_t)evm_module_fs_write)},
		{"writeSync", evm_mk_native((intptr_t)evm_module_fs_writeSync)},
        {"writeFile", evm_mk_native((intptr_t)evm_module_fs_writeFile)},
		{"writeFileSync", evm_mk_native((intptr_t)evm_module_fs_writeFileSync)},
        {NULL, EVM_VAL_UNDEFINED}
	};
	evm_module_create(e, "fs", builtin);
	return e->err;
}
#endif

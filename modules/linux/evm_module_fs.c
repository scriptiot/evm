#ifdef CONFIG_EVM_MODULE_FS
#include "evm_module.h"
#include <rtthread.h>
#include <dfs_fs.h>

//stats.isDirectory()
static evm_val_t evm_module_fs_stats_isDirectory(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//stats.isFile()
static evm_val_t evm_module_fs_stats_isFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
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

//fs.exists
static evm_val_t evm_module_fs_exists(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.existsSync
static evm_val_t evm_module_fs_existsSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.fstat
static evm_val_t evm_module_fs_fstat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.fstatSync
static evm_val_t evm_module_fs_fstatSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.mkdir
static evm_val_t evm_module_fs_mkdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.mkdirSync
static evm_val_t evm_module_fs_mkdirSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.open
static evm_val_t evm_module_fs_open(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.openSync
static evm_val_t evm_module_fs_openSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.read(fd, buffer, offset, length, position, callback)
static evm_val_t evm_module_fs_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
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

//fs.readSync
static evm_val_t evm_module_fs_readSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return evm_module_fs_read(e, p, argc, v);
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

//fs.readFile
static evm_val_t evm_module_fs_readFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.readFileSync
static evm_val_t evm_module_fs_readFileSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.rename
static evm_val_t evm_module_fs_rename(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.renameSync
static evm_val_t evm_module_fs_renameSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.rmdir
static evm_val_t evm_module_fs_rmdir(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.rmdirSync
static evm_val_t evm_module_fs_rmdirSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.stat
static evm_val_t evm_module_fs_stat(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.statSync
static evm_val_t evm_module_fs_statSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.unlink
static evm_val_t evm_module_fs_unlink(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.unlinkSync
static evm_val_t evm_module_fs_unlinkSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.write
static evm_val_t evm_module_fs_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.writeSync
static evm_val_t evm_module_fs_writeSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.writeFile
static evm_val_t evm_module_fs_writeFile(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    return EVM_VAL_UNDEFINED;
}

//fs.writeFileSync
static evm_val_t evm_module_fs_writeFileSync(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
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
		{NULL, NULL}
	};
	evm_module_create(e, "fs", builtin);
	return e->err;
}
#endif

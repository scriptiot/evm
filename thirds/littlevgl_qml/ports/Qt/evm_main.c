#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evm.h"
#include "evm_main.h"
#include "lvgl.h"
#include "lvgl_main.h"


int modules_paths_count = 3;
char* modules_paths[] = {
    ".",
    "./../../../../test/qml",
    "./../../../../test/qml/images/bin/dark",
};

enum FS_MODE{
    FS_READ = 1,
    FS_WRITE = 2,
    FS_APPEND = 4,
    FS_CREATE = 8,
    FS_OPEN = 16,
    FS_TEXT = 32,
    FS_BIN = 64,
};

void * fs_open(char * name, int mode)
{
    char m[5];
    memset(m, 0, 5);
    if( mode & FS_READ)
        sprintf(m, "%sr", m);

    if( mode & FS_WRITE)
        sprintf(m, "%sw", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    if( mode & FS_BIN)
        sprintf(m, "%sb", m);

    if( mode & FS_APPEND)
        sprintf(m, "%sa", m);

    if( mode & FS_TEXT)
        sprintf(m, "%st", m);

    return fopen(name, m);
}

void fs_close(void * handle)
{
    fclose((FILE*)handle);
}

int fs_size(void * handle)
{
    FILE *file = (void*)handle;
    fseek (file , 0 , SEEK_END);
    int lSize = ftell (file);
    rewind (file);
    return lSize;
}

int fs_read(void * handle, char * buf, int len)
{
    return fread (buf, 1, len, (FILE*)handle);
}

int fs_write(void * handle, char * buf, int len)
{
    return fwrite(buf, 1, len, (FILE*)handle);
}

char * loadconfig(char *filename){
    FILE *file;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) return NULL;
    lSize = fs_size(file);
    buffer = (char*)malloc(lSize+1);;
    memset(buffer, 0, lSize + 1);
    result = fs_read(file, buffer, lSize);
    if (!result){
        fclose(file);
        return NULL;
    }
    buffer[lSize] = 0;
    fs_close(file);
    return buffer;
}


char * vm_load_file(evm_t * e, char *filename){
    FILE *file;
    size_t result;
    uint32_t lSize;
    char *buffer = NULL;

    file = fs_open(filename, FS_READ | FS_TEXT);
    if (file == NULL) return NULL;
    lSize = fs_size(file);
    evm_val_t * b = evm_buffer_create(e, sizeof(uint8_t)*lSize + 1);
    buffer = (char*)evm_buffer_addr(b);
    memset(buffer, 0, lSize + 1);
    result = fs_read(file, buffer, lSize);
    if (!result){
        fclose(file);
        return NULL;
    }
    buffer[lSize] = 0;
    fclose(file);
    return buffer;
}

const char * vm_load(evm_t * e, char * path, int type)
{
    int file_name_len = strlen(path) + 1;
    char* buffer = NULL;
    if(type == EVM_LOAD_MAIN){
        char * module_name = evm_malloc(file_name_len);
        if( !module_name ) return NULL;
        sprintf(module_name, "%s", path);
        sprintf(e->file_name, "%s", path);
        buffer = vm_load_file(e, module_name);
        evm_free(module_name);
    } else {
        for(int i=0; i< modules_paths_count; i++){
            int len = strlen(modules_paths[i]) + 1 + file_name_len;
            char* modules_path = evm_malloc(len);
            sprintf(modules_path,  "%s/%s", modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = vm_load_file(e, modules_path);
            evm_free(modules_path);
            if (buffer){
                break;
            }
        }
    }
    return buffer;
}


void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}

void vm_free(void * mem)
{
    if(mem) free(mem);
}


extern int ecma_module(evm_t * e, int num_of_timers);
void ecma_timer_poll(evm_t * e);


extern int qml_lvgl_module(evm_t * e);

int evm_main(char * file)
{
    if (file == NULL){
        file = "../../../../test/qml/watch.qml";
    }
    lvgl_main();

    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);


    int32_t head_size = 10 *1000 * 1024;
    int32_t stack_size = 10000 * 1024;
    int32_t module_size = 10;
    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, head_size, stack_size, module_size, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);


    ecma_module(env, 10);
    if(env->err) {evm_errcode_print(env);evm_deinit(env); return err;}

    err = qml_lvgl_module(env);
    if(err) {evm_errcode_print(env);evm_deinit(env); return err;}
    err = evm_boot(env, file);

    if (err == ec_no_file){
        printf("can't open file '%s': [Errno 2] No such file or directory\n", file);
        return err;
    }

    if(err) {return err;}

    err = evm_start(env);

    lvgl_loop(env);
    return err;
}

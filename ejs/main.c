#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evm.h"
#include "cJSON.h"

#define CHECK_ITEM(item, key) if (!item) { printf("['%s'] is not existed; Please check ejs.json!\n", key); }

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


int modules_paths_count = 0;
char** modules_paths;

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
    fclose(file);
    return buffer;
}


char * open(evm_t * e, char *filename){
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
        buffer = open(e, module_name);
        evm_free(module_name);
    } else {
        for(int i=0; i< modules_paths_count; i++){
            int len = strlen(modules_paths[i]) + 1 + file_name_len;
            char* modules_path = evm_malloc(len);
            sprintf(modules_path,  "%s/%s", modules_paths[i], path);
            sprintf(e->file_name, "%s", path);
            buffer = open(e, modules_path);
            evm_free(modules_path);
            if (buffer){
                break;
            }
        }

        if (!buffer){
            const char * module_path = "../../evm/test/eJS/%s";
            int file_name_len = strlen(module_path) + strlen(path) + 1;
            char * module_name = evm_malloc(file_name_len);
            sprintf(module_name,  module_path, path);
            sprintf(e->file_name, "%s", path);
            buffer = open(e, module_name);
            evm_free(module_name);
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

#ifdef EVM_LANG_ENABLE_JAVASCRIPT
extern int ecma_module(evm_t * e, int num_of_timers);
#endif

#ifdef EVM_LANG_ENABLE_PYTHON
extern int python_builtins(evm_t * e);
#endif


void help(void)
{
    printf(QMAKE_TARGET " version " QMAKE_VERSION "\n"
           "usage: " QMAKE_TARGET " [file.js]\n"
    );
    exit(1);
}


int main(int argc, char *argv[])
{
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    if (argc == 1){
        help();
    }

    int32_t head_size = 10 *1000 * 1024;
    int32_t stack_size = 10000 * 1024;
    int32_t module_size = 10;
    char *config;
    config = loadconfig(QMAKE_TARGET".json");
    if( config ){
        cJSON * root = NULL;
        cJSON * item = NULL;//cjson对象
        root = cJSON_Parse(config);
        if (!root)
        {
            printf(QMAKE_TARGET".json parser error: [%s]\n",cJSON_GetErrorPtr());
        }
        else
        {
            item = cJSON_GetObjectItem(root, "heap_size");
            CHECK_ITEM(item, "heap_size")
            head_size = item->valueint;
            item = cJSON_GetObjectItem(root, "stack_size");
            CHECK_ITEM(item, "stack_size")
            stack_size = item->valueint;
            item = cJSON_GetObjectItem(root, "module_size");
            CHECK_ITEM(item, "module_size")
            module_size = item->valueint;
            item = cJSON_GetObjectItem(root, "module_paths");
            CHECK_ITEM(item, "module_paths")
            if (cJSON_IsArray(item)){
                modules_paths_count = cJSON_GetArraySize(item);
                modules_paths = malloc(sizeof(char*)*modules_paths_count);
                for(int i=0; i< modules_paths_count; i++){
                    cJSON * pathItem = cJSON_GetArrayItem(item, i);
                    modules_paths[i] = cJSON_GetStringValue(pathItem);
                }
            }
        }
        memset(config, 0, sizeof(config));
    }
    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, head_size, stack_size, module_size, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

#ifdef EVM_LANG_ENABLE_JAVASCRIPT
    ecma_module(env, 10);
    if(env->err) {evm_errcode_print(env);evm_deinit(env); return err;}
#endif

#ifdef EVM_LANG_ENABLE_PYTHON
    python_builtins(env);
    if(env->err) {evm_errcode_print(env);evm_deinit(env); return err;}
#endif


    err = evm_boot(env, argv[1]);

    if (err == ec_no_file){
        printf(QMAKE_TARGET": can't open file '%s': [Errno 2] No such file or directory\n", argv[1]);
        exit(1);
    }

    if(err) {return err;}

    err = evm_start(env);

    return err;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "evm.h"

/*****************REPL*******************/
// 定义REPL接口函数evm_repl_tty_read，从tty终端获取字符
#ifdef EVM_LANG_ENABLE_REPL
#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#endif

#ifdef __WIN64__
#include <conio.h>
#endif

#ifdef __linux__
/**
 * @brief linux平台终端repl读取单个字符接口
 * @return 单个字符
 */
char mygetch(void)  // 不回显获取字符
{
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    newt.c_cc[VEOL] = 1;
    newt.c_cc[VEOF] = 2;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

#ifdef __WIN64__
/**
 * @brief windows平台终端repl读取单个字符接口
 * @return 单个字符
 */
char mygetch(void)  // 不回显获取字符
{
    return getch();
}

#endif

// 如果启动REPL, 必须实现evm_repl_tty_read接口，
/**
 * @brief 终端repl读取单个字符接口, 如果是单片机实时调试，基于串口读取字符实现
 * @param e, 虚拟机对象
 * @return 单个字符
 */
char evm_repl_tty_read(evm_t * e)
{
    return mygetch();
}
#endif


/******************文件操作API******************/
enum FS_MODE{
    FS_READ = 1,
    FS_WRITE = 2,
    FS_APPEND = 4,
    FS_CREATE = 8,
    FS_OPEN = 16,
    FS_TEXT = 32,
    FS_BIN = 64,
};

/**
 * @brief 打开文件
 * @param name, 文件路径
 * @param mode, 文件打开模式FS_MODE
 * @return 文件句柄
 */
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

/**
 * @brief 关闭文件
 * @param handle, 文件句柄
 * @return
 */
void fs_close(void * handle)
{
    fclose((FILE*)handle);
}

/**
 * @brief 读取文件内容长度
 * @param handle, 文件句柄
 * @return 文件内容长度
 */
int fs_size(void * handle)
{
    FILE *file = (void*)handle;
    fseek (file , 0 , SEEK_END);
    int lSize = ftell (file);
    rewind (file);
    return lSize;
}

/**
 * @brief 读取文件内容
 * @param handle, 文件句柄
 * @param buf, 文件内容缓存buf
 * @param len, 读取字节长度
 * @return 读取字节长度
 */
int fs_read(void * handle, char * buf, int len)
{
    return fread (buf, 1, len, (FILE*)handle);
}

/**
 * @brief 读取文件内容
 * @param e, 虚拟机对象
 * @param path, 脚本文件路径
 * @return buffer, 返回脚本文件内容
 */
int fs_write(void * handle, char * buf, int len)
{
    return fwrite(buf, 1, len, (FILE*)handle);
}

/**
 * @brief 读取文件内容
 * @param e, 虚拟机对象
 * @param path, 脚本文件路径
 * @return buffer, 返回脚本文件内容
 */
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

/*****************定义模块寻址路径列表和路径个数*******************/
/**
 * modules_paths_count 模块路径个数
 * modules_paths 模块路径列表
 */
int modules_paths_count = 2;
char* modules_paths[] = {
    ".",
    "./evm_modules"
};


/*****************定义虚拟机注册API*******************/

/**
 * @brief 加载main运行脚本和加模块脚本
 * @param e, 虚拟机对象
 * @param path, 脚本路径
 * @param type, EVM_LOAD_MAIN代表main运行脚本，非EVM_LOAD_MAIN代表加载模块
 * @return
 */
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

/**
 * @brief 外部内存申请接口
 * @param size, 申请内存大小
 * @return 内存成功分配的对象指针
 */
void * vm_malloc(int size)
{
    void * m = malloc(size);
    if(m) memset(m, 0 ,size);
    return m;
}

/**
 * @brief 外部内存释放接口
 * @param mem, 需要释放的对象指针
 * @return
 */
void vm_free(void * mem)
{
    if(mem) free(mem);
}

/****************声明ecma模块********************/

#ifdef EVM_LANG_ENABLE_JAVASCRIPT
extern int ecma_module(evm_t * e, int num_of_timers);
#endif

/****************声明python内置模块***************/

#ifdef EVM_LANG_ENABLE_PYTHON
extern int python_builtins(evm_t * e);
#endif

#define QMAKE_TARGET "ejs"
#define QMAKE_VERSION "1.0"

/****************终端提示信息***************/
void help(void)
{
    printf(QMAKE_TARGET " version " QMAKE_VERSION "\n"
           "usage: " QMAKE_TARGET " [file.js]\n"
    );
}


int main(int argc, char *argv[])
{
    // 注册平台相关的虚拟机API
    evm_register_free((intptr_t)vm_free);
    evm_register_malloc((intptr_t)vm_malloc);
    evm_register_print((intptr_t)printf);
    evm_register_file_load((intptr_t)vm_load);

    // 初始化虚拟机
    int32_t head_size = 10 *1000 * 1024;
    int32_t stack_size = 10000 * 1024;
    int32_t module_size = 10;
    evm_t * env = (evm_t*)malloc(sizeof(evm_t));
    memset(env, 0, sizeof(evm_t));
    int err = evm_init(env, head_size, stack_size, module_size, EVM_VAR_NAME_MAX_LEN, EVM_FILE_NAME_LEN);

    // 加载ecma模块
#ifdef EVM_LANG_ENABLE_JAVASCRIPT
    ecma_module(env, 10);
    if(env->err) {evm_errcode_print(env);evm_deinit(env); return err;}
#endif

    // 加载python内置模块
#ifdef EVM_LANG_ENABLE_PYTHON
    python_builtins(env);
    if(env->err) {evm_errcode_print(env);evm_deinit(env); return err;}
#endif

    // 启动REPL调试
    if (argc == 1){
        help();
#ifdef EVM_LANG_ENABLE_REPL
#ifdef EVM_LANG_ENABLE_JAVASCRIPT
        evm_repl_run(env, 1000, EVM_LANG_JS);
#endif
#ifdef EVM_LANG_ENABLE_PYTHON
        evm_repl_run(env, 1000, EVM_LANG_PY);
#endif
#endif
    }

    // 加载js/py/lua等脚本文件
    err = evm_boot(env, argv[1]);

    if (err == ec_no_file){
        printf(QMAKE_TARGET": can't open file '%s': [Errno 2] No such file or directory\n", argv[1]);
        exit(1);
    }

    if(err) {return err;}

    // 启动虚拟机
    err = evm_start(env);

    return err;
}

#ifndef __HTTP_CLIENT_ARCH_H__
#define __HTTP_CLIENT_ARCH_H__

//#define http_debug(fmt, args...) do { CPUartLogPrintf("[http_client]"fmt, ##args); } while(0)
//#define http_error(fmt, args...) do { CPUartLogPrintf("[http_client]"fmt, ##args); } while(0)

extern char * __http_client_strdup(const char *s);

extern void * __http_client_malloc(int size);

extern void __http_client_free(void * p);

#define HTTP_CLIENT_MALLOC __http_client_malloc
#define HTTP_CLIENT_FREE(p)   do{__http_client_free(p);p=NULL;}while(0)

#undef strdup
#define strdup __http_client_strdup

#endif

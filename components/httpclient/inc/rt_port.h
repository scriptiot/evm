#ifndef RT_PORT_H
#define RT_PORT_H

#include <stdarg.h>
#include <errno.h>

typedef int rt_bool_t;
typedef int rt_int32_t;

#define http_debug printf
#define http_error printf

#define RT_USING_SAL                    "yes"
#define closesocket(s) close(s)

#define RT_NULL NULL
#define RT_FALSE 0
#define RT_TRUE 1

#define RT_ASSERT(x) ((void) (x))
#define LOG_E printf
#define LOG_D printf

#define rt_strlen strlen
#define rt_strstr strstr
#define rt_strncpy strncpy
#define rt_vsnprintf vsnprintf
#define rt_memset memset
#define rt_snprintf snprintf
#define rt_strcmp strcmp
#define rt_tick_from_millisecond (int)


#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */
#define RT_ETRAP                        11              /**< trap event */

#endif

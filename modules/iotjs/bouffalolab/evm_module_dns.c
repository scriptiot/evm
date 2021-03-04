#ifdef CONFIG_EVM_MODULE_DNS
#include "evm_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

//dns.lookup(hostname[, options], callback)
static evm_val_t evm_module_dns_lookup(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    struct hostent *hostinfo;
    const char *hostname = evm_2_string(v);
    hostinfo = gethostbyname(hostname);

    if (argc > 1 && evm_is_script(v + 1))
    {
        evm_val_t args[4];
        args[0] = *(v + 1);
        args[1] = evm_mk_number(errno);
        evm_val_t *host_ip = evm_heap_string_create(e, inet_ntoa(*((struct in_addr *)hostinfo->h_addr_list[0])), 0);
        if (host_ip)
            args[2] = *host_ip;
        else
            args[2] = evm_mk_foreign_string("");
        args[3] = evm_mk_number(hostinfo->h_addrtype);
        evm_module_next_tick(e, 4, args);
    }
    return EVM_VAL_UNDEFINED;
}

evm_err_t evm_module_dns(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"lookup", evm_mk_native((intptr_t)evm_module_dns_lookup)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "dns", builtin);
    return e->err;
}

#endif

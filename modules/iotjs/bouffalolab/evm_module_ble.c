#ifdef CONFIG_EVM_MODULE_BLE
#include "evm_module.h"
#include <stdio.h>
#include <stdlib.h>

//ble.init(hostname[, options], callback)
static evm_val_t evm_module_dns_lookup(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    return EVM_VAL_UNDEFINED;
}

//ble.start
//ble.stop
//ble.auth
//ble.cancelAuth
//ble.confirmAuthPasskey
//ble.confirmAuthPairing
//ble.setAuthPasskey
//ble.setMtuExchange
//ble.discover
//ble.read
//ble.write
//ble.writeWithNoReponse
//ble.subscribe
//ble.unsubscribe
//ble.setPduLength
//ble.getConnection
//ble.disable
//ble.setPower
//ble.unpair
//ble.startAdvertising
//ble.stopAdvertising
//ble.startScan
//ble.stopScan
//ble.updateConnection
//ble.getDeviceName
//ble.setDeviceName
//ble.readLocalAddress
//ble.setAdvertisingChannel
//ble.connect
//ble.disconnect
//ble.selectConnection

evm_err_t evm_module_dns(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"lookup", evm_mk_native((intptr_t)evm_module_dns_lookup)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "dns", builtin);
    return e->err;
}

#endif

#include "evm_module.h"

void evm_module_next_tick(evm_t *e, int argc, evm_val_t *v) {
#ifdef CONFIG_EVM_MODULE_PROCESS
    evm_module_process_nextTick(e, NULL, argc, v);
#endif
}

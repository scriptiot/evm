#include "evm_module.h"


//CAN(name)
static evm_val_t evm_module_can(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.initfilterbanks(nr)
static evm_val_t evm_module_can_initfilterbanks(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.init(mode, extframe=False, prescaler=100, *, sjw=1, bs1=6, bs2=8, auto_restart=False)
static evm_val_t evm_module_can_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.deinit()
static evm_val_t evm_module_can_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.restart()
static evm_val_t evm_module_can_restart(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.state()
static evm_val_t evm_module_can_state(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.info([list])
static evm_val_t evm_module_can_info(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.setfilter(bank, mode, fifo, params, *, rtr)
static evm_val_t evm_module_can_setfilter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.clearfilter(bank)
static evm_val_t evm_module_can_clearfilter(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.any(fifo)
static evm_val_t evm_module_can_any(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.recv(fifo, list=None, *, timeout=5000)
static evm_val_t evm_module_can_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.send(data, id, *, timeout=0, rtr=False)
static evm_val_t evm_module_can_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}
//CAN.rxcallback(fifo, fun)
static evm_val_t evm_module_can_rxcallback(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_can(evm_t * e){
	evm_builtin_t class_can[] = {
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_can, class_can, NULL);
}


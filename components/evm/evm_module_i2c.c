#include "evm_module.h"

//I2C(bus, ...)
static evm_val_t evm_module_i2c(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.deinit()
static evm_val_t evm_module_i2c_deinit(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.init(mode, *, addr=0x12, baudrate=400000, gencall=False, dma=False)
static evm_val_t evm_module_i2c_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.is_ready(addr)
static evm_val_t evm_module_i2c_is_ready(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.mem_read(data, addr, memaddr, *, timeout=5000, addr_size=8)
static evm_val_t evm_module_i2c_mem_read(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.mem_write(data, addr, memaddr, *, timeout=5000, addr_size=8)
static evm_val_t evm_module_i2c_mem_write(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.recv(recv, addr=0x00, *, timeout=5000)
static evm_val_t evm_module_i2c_recv(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.send(send, addr=0x00, *, timeout=5000)
static evm_val_t evm_module_i2c_send(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

//I2C.scan()
static evm_val_t evm_module_i2c_scan(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_i2c(evm_t * e){
	evm_builtin_t class_i2c[] = {
		{"deinit", evm_mk_native( (intptr_t)evm_module_i2c_deinit )},
		{"init", evm_mk_native( (intptr_t)evm_module_i2c_init )},
		{"is_ready", evm_mk_native( (intptr_t)evm_module_i2c_is_ready )},
		{"mem_read", evm_mk_native( (intptr_t)evm_module_i2c_mem_read )},
		{"mem_write", evm_mk_native( (intptr_t)evm_module_i2c_mem_write )},
		{"recv", evm_mk_native( (intptr_t)evm_module_i2c_recv )},
		{"send", evm_mk_native( (intptr_t)evm_module_i2c_send )},
		{"scan", evm_mk_native( (intptr_t)evm_module_i2c_scan )},
		{NULL, NULL}
	};
	return *evm_class_create(e, (evm_native_fn)evm_class_i2c, class_i2c, NULL);
}


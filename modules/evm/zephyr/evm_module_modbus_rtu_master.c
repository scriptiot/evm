/****************************************************************************
**
** Copyright (C) 2020 @scriptiot
**
**  EVM是一款通用化设计的虚拟机引擎，拥有语法解析前端接口、编译器、虚拟机和虚拟机扩展接口框架。
**  支持js、python、qml、lua等多种脚本语言，纯Ｃ开发，零依赖，内置REPL，支持主流 ROM > 40KB, RAM > 2KB的MCU;
**  自带垃圾回收（GC）先进的内存管理，采用最复杂的压缩算法，无内存碎片（大部分解释器都存在内存碎片）
**  Version	: 1.0
**  Email	: scriptiot@aliyun.com
**  Website	: https://github.com/scriptiot/evm
**            https://gitee.com/scriptiot/evm
**  Licence: Apache-2.0
****************************************************************************/


#include "evm_module.h"

#include <drivers/uart.h>

#define MB_ADDRESS_BROADCAST                  ( 0 )   
#define MB_ADDRESS_MIN                        ( 1 )   
#define MB_ADDRESS_MAX                        ( 247 ) 
#define MB_FUNC_NONE                          (  0 )
#define MB_FUNC_READ_COILS                    (  1 )
#define MB_FUNC_READ_DISCRETE_INPUTS          (  2 )
#define MB_FUNC_WRITE_SINGLE_COIL             (  5 )
#define MB_FUNC_WRITE_MULTIPLE_COILS          ( 15 )
#define MB_FUNC_READ_HOLDING_REGISTER         (  3 )
#define MB_FUNC_READ_INPUT_REGISTER           (  4 )
#define MB_FUNC_WRITE_REGISTER                (  6 )
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS      ( 16 )
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS  ( 23 )
#define MB_FUNC_DIAG_READ_EXCEPTION           (  7 )
#define MB_FUNC_DIAG_DIAGNOSTIC               (  8 )
#define MB_FUNC_DIAG_GET_COM_EVENT_CNT        ( 11 )
#define MB_FUNC_DIAG_GET_COM_EVENT_LOG        ( 12 )
#define MB_FUNC_OTHER_REPORT_SLAVEID          ( 17 )
#define MB_FUNC_ERROR                         ( 128 )

typedef enum
{
    MB_EX_NONE = 0x00,
    MB_EX_ILLEGAL_FUNCTION = 0x01,
    MB_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MB_EX_ILLEGAL_DATA_VALUE = 0x03,
    MB_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MB_EX_ACKNOWLEDGE = 0x05,
    MB_EX_SLAVE_BUSY = 0x06,
    MB_EX_MEMORY_PARITY_ERROR = 0x08,
    MB_EX_GATEWAY_PATH_FAILED = 0x0A,
    MB_EX_GATEWAY_TGT_FAILED = 0x0B
} MBException;

#define MB_UINT16_MAX               0xFFFF
#define MB_UINT16_MIN               0x0000
#define MB_UINT8_MAX                0xFF

#define MB_MAX_RTU_MESSAGE_LENGTH       256
#define MB_MAX_ASCII_MESSAGE_LENGTH     517

#define MB_MAX_ID                   255
#define MB_MAX_ADDRESS              65535
#define MB_MAX_REG_LEN              65535

#define MB_OK                       0
#define MB_ERR_ID                   1
#define MB_ERR_ADDRESS              2
#define MB_ERR_REG_LEN              3
#define MB_ERR_MAX_RTU_MSG_LEN      4

typedef enum
{
    MB_REG_READ,
    MB_REG_WRITE              
} MBRegisterMode;

typedef struct mb_rtu_master_t {
    struct k_timer * timer;
    struct k_timer * timeout;
    struct device * dev;
    uint32_t time;
    uint8_t * rx_buf;
    uint16_t rx_count;
    uint8_t * tx_buf;
    uint32_t duration;
    uint8_t id;
    int obj;
    int reg_callback;
    int input_callback;
    int discrete_callback;
    int coils_callback;
} mb_rtu_master_t;

int16_t _mb_crc(uint8_t * data, uint16_t len){
    int16_t crc = 0xffff;
    int8_t i,j;
    for( i = 0; i < len; i++){
        crc = crc ^ data[i];
        for( j = 0; j < 8; j++){
            if( (crc & 0x0001) == 1 ){
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}
//callback(len, mode)
void _mb_rtu_m_holding_register_cb(mb_rtu_master_t * rtu_m, uint16_t nRegs, MBRegisterMode mode){
    if( rtu_m->reg_callback == -1 ) return;
    evm_val_t * cb_fn = evm_get_reference(rtu_m->reg_callback);
    if( !evm_is_script(cb_fn) ) return;

    evm_val_t vals[2];
    vals[0] = evm_mk_number(nRegs);
    vals[1] = evm_mk_number(mode);

    evm_run_callback(evm_runtime, cb_fn, &evm_runtime->scope, vals, 2);
}
//callback(len, mode)
void _mb_rtu_m_coils_cb(mb_rtu_master_t * rtu_m, uint16_t nRegs, MBRegisterMode mode){
    if( rtu_m->coils_callback == -1 ) return;
    evm_val_t * cb_fn = evm_get_reference(rtu_m->coils_callback);
    if( !evm_is_script(cb_fn) ) return;

    evm_val_t vals[2];
    vals[0] = evm_mk_number(nRegs);
    vals[1] = evm_mk_number(mode);

    evm_run_callback(evm_runtime, cb_fn, &evm_runtime->scope, vals, 2);
}
//callback(len)
void _mb_rtu_m_input_cb(mb_rtu_master_t * rtu_m, uint16_t nRegs){
    if( rtu_m->input_callback == -1 ) return;
    evm_val_t * cb_fn = evm_get_reference(rtu_m->input_callback);
    if( !evm_is_script(cb_fn) ) return;

    evm_val_t vals[1];
    vals[0] = evm_mk_number(nRegs);

    evm_run_callback(evm_runtime, cb_fn, &evm_runtime->scope, vals, 1);
}
//callback(len)
void _mb_rtu_m_discrete_cb(mb_rtu_master_t * rtu_m, uint16_t nRegs){
    if( rtu_m->discrete_callback == -1 ) return;
    evm_val_t * cb_fn = evm_get_reference(rtu_m->discrete_callback);
    if( !evm_is_script(cb_fn) ) return;

    evm_val_t vals[1];
    vals[0] = evm_mk_number(nRegs);

    evm_run_callback(evm_runtime, cb_fn, &evm_runtime->scope, vals, 1);
}

static void _mb_rtu_m_timeout_cb(struct k_timer *handle){

}

static void _mb_rtu_m_timer_cb(struct k_timer *handle)
{
    mb_rtu_master_t * rtu_master = (mb_rtu_master_t*)handle->user_data;
    //goto error callback
    if( rtu_master->rx_count == 0 )  {
        goto CB_END;
    }
    //get function code
    if( rtu_master->rx_count >= 2 ) {
        //goto error callback
        if( rtu_master->id != rtu_master->rx_buf[0] ) {
            goto CB_END;
        }

        uint8_t func_code = rtu_master->rx_buf[1];
        uint16_t nRegs;
        switch(func_code){
            case MB_FUNC_READ_DISCRETE_INPUTS:
                nRegs = rtu_master->rx_buf[2];
                _mb_rtu_m_discrete_cb(rtu_master, nRegs);break;
            case MB_FUNC_READ_COILS:
                nRegs = rtu_master->rx_buf[2];
                _mb_rtu_m_coils_cb(rtu_master, nRegs, MB_REG_READ);break;
            case MB_FUNC_WRITE_SINGLE_COIL:
            case MB_FUNC_WRITE_MULTIPLE_COILS:
                nRegs = rtu_master->rx_buf[2];
                _mb_rtu_m_coils_cb(rtu_master, nRegs, MB_REG_WRITE);break;
            case MB_FUNC_READ_INPUT_REGISTER:
                nRegs = rtu_master->rx_buf[2];
                _mb_rtu_m_input_cb(rtu_master, nRegs);break;
            case MB_FUNC_WRITE_REGISTER:
            case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
            case MB_FUNC_READWRITE_MULTIPLE_REGISTERS:
                nRegs = rtu_master->rx_buf[2] / 2;
                _mb_rtu_m_holding_register_cb(rtu_master, nRegs, MB_REG_WRITE);break;
            case MB_FUNC_READ_HOLDING_REGISTER:
                nRegs = rtu_master->rx_buf[2] / 2;
                _mb_rtu_m_holding_register_cb(rtu_master, nRegs, MB_REG_READ);break;
            default://goto error callback
                goto CB_END;
        }
    }
    CB_END:
        rtu_master->rx_count = 0;
}

int _mb_rtu_read_holding_register(mb_rtu_master_t * rtu_master, 
                                int id, 
                                int address, 
                                int len){
    if( id > MB_MAX_ID || id < 0 ) return MB_ERR_ID;
    if( address > MB_MAX_ADDRESS || address < 0 ) return MB_ERR_ADDRESS;
    if( len > MB_MAX_REG_LEN || len < 0 ) return MB_ERR_REG_LEN;
    rtu_master->id = id;
    rtu_master->tx_buf[0] = id;
    rtu_master->tx_buf[1] = MB_FUNC_READ_HOLDING_REGISTER;
    rtu_master->tx_buf[2] = address >> 8;
    rtu_master->tx_buf[3] = address;
    rtu_master->tx_buf[4] = len >> 8;
    rtu_master->tx_buf[5] = len;
    int16_t crc = _mb_crc(rtu_master->tx_buf, 6);
    rtu_master->tx_buf[6] = crc >> 8;
    rtu_master->tx_buf[7] = crc;

    for (int i = 0; i < 8; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    return MB_OK;
}

int _mb_rtu_write_holding_register(mb_rtu_master_t * rtu_master, 
                                     int id, 
                                     int address, 
                                     uint16_t value){
    if( id > MB_MAX_ID || id < 0 ) return MB_ERR_ID;
    if( address > MB_MAX_ADDRESS || address < 0 ) return MB_ERR_ADDRESS;

    rtu_master->id = id;
    rtu_master->tx_buf[0] = id;
    rtu_master->tx_buf[1] = MB_FUNC_WRITE_REGISTER;
    rtu_master->tx_buf[2] = address >> 8;
    rtu_master->tx_buf[3] = address;
    rtu_master->tx_buf[4] = value >> 8;
    rtu_master->tx_buf[5] = value;
    int16_t crc = _mb_crc(rtu_master->tx_buf, 6);
    rtu_master->tx_buf[6] = crc >> 8;
    rtu_master->tx_buf[7] = crc;

    for (int i = 0; i < 9; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    return MB_OK;
}

int _mb_rtu_write_multiple_holding_register(mb_rtu_master_t * rtu_master, 
                                     int id, 
                                     int address, 
                                     int len, 
                                     uint16_t * values){
    if( id > MB_MAX_ID || id < 0 ) return MB_ERR_ID;
    if( address > MB_MAX_ADDRESS || address < 0 ) return MB_ERR_ADDRESS;
    if( len > MB_MAX_REG_LEN || len < 0 ) return MB_ERR_REG_LEN;
    if( len * 2 > MB_MAX_RTU_MESSAGE_LENGTH - 9) return MB_ERR_MAX_RTU_MSG_LEN;

    rtu_master->id = id;
    rtu_master->tx_buf[0] = id;
    rtu_master->tx_buf[1] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
    rtu_master->tx_buf[2] = address >> 8;
    rtu_master->tx_buf[3] = address;
    rtu_master->tx_buf[4] = len >> 8;
    rtu_master->tx_buf[5] = len;
    rtu_master->tx_buf[6] = len * 2;
    for( int i = 0; i < len; i++){
        rtu_master->tx_buf[7 + (2 * i)] = values[i] >> 8;
        rtu_master->tx_buf[8 + (2 * i)] = values[i];
    }
    int16_t crc = _mb_crc(rtu_master->tx_buf, 7 + len * 2);
    rtu_master->tx_buf[7 + len * 2] = crc >> 8;
    rtu_master->tx_buf[8 + len * 2] = crc;

    int data_len = 9 + len * 2;

    for (int i = 0; i < data_len; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    return MB_OK;
}

static void uart_irq_handler(mb_rtu_master_t * handle)
{
    uart_irq_update(handle->dev);

    if (!uart_irq_is_pending(handle->dev)) {
        return;
    }

    if (uart_irq_tx_ready(handle->dev)) {
        
    }

    if (uart_irq_rx_ready(handle->dev)) {
        char recvData;
		uart_fifo_read(handle->dev, &recvData, 1);
        handle->rx_count++;
        k_timer_start(handle->timer, K_MSEC(handle->duration), K_NO_WAIT);
    }
}
//ModbusRTUMaster(devName, baudrate)
static evm_val_t evm_module_modbus_rtu_master(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 1 && evm_is_string(v) && evm_is_number(v + 1) ){
        struct device * dev = (struct device *)device_get_binding(evm_2_string(v));
        if(!dev) {
            evm_set_err(e, ec_type, "Can't find uart device");
		    return EVM_VAL_UNDEFINED;
        }

        struct uart_config uart_cfg = {
            .baudrate = evm_2_integer(v + 1),
            .data_bits = UART_CFG_DATA_BITS_8,
            .parity = UART_CFG_PARITY_NONE,
            .stop_bits = UART_CFG_STOP_BITS_1,
            .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
        };

        uart_configure(dev, &uart_cfg);
        mb_rtu_master_t * rtu_master = evm_malloc(sizeof(mb_rtu_master_t));
        rtu_master->dev = dev;
        rtu_master->obj = evm_add_reference(*p);
        uart_irq_callback_user_data_set(dev, (uart_irq_callback_user_data_t)uart_irq_handler, rtu_master);
        uart_irq_rx_enable(dev);

        rtu_master->timer = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
        if( !rtu_master->timer ) {
            evm_set_err(e, ec_type, "Can't create timer for uart");
            return EVM_VAL_UNDEFINED;
        }

        rtu_master->timeout = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
        if( !rtu_master->timeout ) {
            evm_set_err(e, ec_type, "Can't create timer for timeout");
            return EVM_VAL_UNDEFINED;
        }

        k_timer_init(rtu_master->timer, _mb_rtu_m_timer_cb, NULL);
        rtu_master->timer->user_data = rtu_master;

        evm_object_set_ext_data(p, (intptr_t)rtu_master);
    }

	return EVM_VAL_UNDEFINED;
}
//read_registers(id, address, number, callback)
static evm_val_t evm_module_modbus_rtu_master_read_registers(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	if( argc > 3 && evm_is_number(v) && evm_is_number(v + 1) && evm_is_number(v + 2) && evm_is_script(v + 3) ){
        mb_rtu_master_t * rtu_master = (mb_rtu_master_t * )evm_object_get_ext_data(p);
        if( rtu_master->reg_callback != -1 ) evm_remove_reference(rtu_master->reg_callback);
        rtu_master->reg_callback = evm_add_reference(*(v + 3));

        int id = evm_2_integer(v);
        int addr = evm_2_integer(v + 1);
        int num = evm_2_integer(v + 2);
        if( num < 1 ) return EVM_VAL_FALSE;

        _mb_rtu_read_holding_register(rtu_master, id, addr, num);
        
    }
    return EVM_VAL_UNDEFINED;
}
//write_register(id, address, values, callback)
static evm_val_t evm_module_modbus_rtu_master_write_registers(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc > 3 && evm_is_number(v) && evm_is_number(v + 1) ){
        mb_rtu_master_t * rtu_master = (mb_rtu_master_t * )evm_object_get_ext_data(p);
        int id = evm_2_integer(v);
        int addr = evm_2_integer(v + 1);
        if( argc > 2 && evm_is_number(v + 2) ) {
            if(  argc == 4 && evm_is_script(v + 3) ) {
                if( rtu_master->reg_callback != -1 ) evm_remove_reference(rtu_master->reg_callback);
                rtu_master->reg_callback = evm_add_reference(*(v + 3));
            }

            if( argc == 3 ) {
                int value = evm_2_integer(v + 2);
                if( _mb_rtu_write_holding_register(rtu_master, id, addr, value) != MB_OK ) return EVM_VAL_FALSE;
            } else {
                return EVM_VAL_FALSE;
            }
        } else if( evm_is_number(v + 2) && evm_is_list(v + 3) ) {
            if( argc == 5 && evm_is_script(v + 4) ) {
                if( rtu_master->reg_callback != -1 ) evm_remove_reference(rtu_master->reg_callback);
                rtu_master->reg_callback = evm_add_reference(*(v + 4));
            }
            
            int len = evm_2_integer(v + 2);
            int list_len = evm_list_len(v + 3);
            if(len > list_len) len = list_len;
            if( len ) {
                uint16_t * values = evm_malloc(sizeof(uint16_t) * len);
                for(int i = 0; i < len; i++){
                    values[i] = evm_2_integer(evm_list_get(e, v + 3, i));
                }
                _mb_rtu_write_multiple_holding_register(rtu_master, id, addr, len, values);
                evm_free(values);
            } else return EVM_VAL_FALSE;
        } else if( evm_is_number(v + 2) && evm_is_buffer(v + 3) && evm_is_script(v + 4) ) {
            if( argc == 5 && evm_is_script(v + 4) ) {
                if( rtu_master->reg_callback != -1 ) evm_remove_reference(rtu_master->reg_callback);
                rtu_master->reg_callback = evm_add_reference(*(v + 4));
            }

            int len = evm_2_integer(v + 2);
            int list_len = evm_buffer_len(v + 3);
            if(len > (list_len / 2) ) len = (list_len / 2);
            if( len ) {
                uint16_t * values = evm_malloc(sizeof(uint16_t) * len);
                uint8_t * buf = evm_buffer_addr(v + 3);
                for(int i = 0; i < len; i++){
                    values[i] = (buf[ i * 2 ] << 8) | buf[ i * 2 + 1];
                }
                _mb_rtu_write_multiple_holding_register(rtu_master, id, addr, len, values);
                evm_free(values);
            } else return EVM_VAL_FALSE;
        } else {
            return EVM_VAL_FALSE;
        }
    }
    return EVM_VAL_TRUE;
}
//get_values(list|buffer)
static evm_val_t evm_module_modbus_rtu_master_get_values(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	mb_rtu_master_t * rtu_master = (mb_rtu_master_t * )evm_object_get_ext_data(p);
    if( rtu_master->rx_count == 0 ) {
        return EVM_VAL_UNDEFINED;
    }
    uint8_t func_code = rtu_master->rx_buf[1];
    uint8_t num_of_bytes = rtu_master->rx_buf[2];
    if( argc == 0 ) {
        switch(func_code) {
            case MB_FUNC_READ_DISCRETE_INPUTS: return evm_mk_number(rtu_master->rx_buf[3]);
            case MB_FUNC_READ_COILS: return evm_mk_number(rtu_master->rx_buf[3]);
            case MB_FUNC_READ_INPUT_REGISTER:
                return evm_mk_number( rtu_master->rx_buf[3] );
            case MB_FUNC_READ_HOLDING_REGISTER:
                return evm_mk_number( (rtu_master->rx_buf[3] << 8) | rtu_master->rx_buf[4] );
            default: return EVM_VAL_UNDEFINED;
        }
    } else if( evm_is_list(v) ) {
        switch(func_code) {
            case MB_FUNC_READ_DISCRETE_INPUTS:
            case MB_FUNC_READ_COILS:
            case MB_FUNC_READ_INPUT_REGISTER:  {
                int len = evm_list_len(v);
                if( num_of_bytes < len ) len = num_of_bytes;
                for(int i = 0; i < len; i++) {
                    evm_list_set(e, v, i, evm_mk_number(  rtu_master->rx_buf[3 + i]) );
                }
            } break;
            case MB_FUNC_READ_HOLDING_REGISTER: {
                int len = evm_list_len(v);
                if( num_of_bytes/2 < len ) len = num_of_bytes/2;
                for(int i = 0; i < len; i++) {
                    evm_list_set(e, v, i, evm_mk_number((rtu_master->rx_buf[3 + i*2] << 8) | rtu_master->rx_buf[4 + i*2] ) );
                }
            } break;
            default: return EVM_VAL_UNDEFINED;
        }
    } else if( evm_is_buffer(v) ) {
        switch(func_code) {
            case MB_FUNC_READ_DISCRETE_INPUTS:
            case MB_FUNC_READ_COILS:
            case MB_FUNC_READ_INPUT_REGISTER: 
            case MB_FUNC_READ_HOLDING_REGISTER: {
                int len = evm_buffer_len(v);
                if( num_of_bytes < len ) len = num_of_bytes;
                uint8_t * addr = evm_buffer_addr(v);
                memcpy(addr, rtu_master->rx_buf + 3, len);
            } break;
            default: return EVM_VAL_UNDEFINED;
        }
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_modbus_rtu(evm_t *e)
{
	evm_builtin_t clazz[] = {
		{"read_registers", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_read_registers)},
        {"write_registers", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_write_registers)},
        {"get_values", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_get_values)},
		{NULL, EVM_VAL_UNDEFINED}};
	return *evm_class_create(e, (evm_native_fn)evm_module_modbus_rtu_master, clazz, NULL);
}


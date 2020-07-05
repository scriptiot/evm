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
    struct device * dev;
    uint8_t * rx_buf;
    uin16_t rx_count;
    uint8_t * tx_buf;
    uint32_t duration;
    uint8_t id;
    int obj;
    int holding_reg_cb;
    int err_cb;
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
                crc = crc >> 1
            }
        }
    }
    return crc;
}

void _mb_rtu_m_holding_register_cb(uint8_t * buf, uint16_t address, uint16_t nRegs, MBRegisterMode mode){

}

void _mb_rtu_m_coils_cb(uint8_t * buf, uint16_t address, uint16_t nRegs, MBRegisterMode mode){

}

void _mb_rtu_m_input_cb(uint8_t * buf, uint16_t address, uint16_t nRegs){

}

void _mb_rtu_m_discrete_cb(uint8_t * buf, uint16_t address, uint16_t nRegs){

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
        uint16_t address;
        uint16_t nRegs;
        switch(func_code){
            case MB_FUNC_READ_DISCRETE_INPUTS:
                _mb_rtu_m_discrete_cb(rtu_master->rx_buf, address, nRegs);break;
            case MB_FUNC_READ_COILS:
                _mb_rtu_m_coils_cb(rtu_master->rx_buf, address, nRegs, MB_REG_READ);break;
            case MB_FUNC_WRITE_SINGLE_COIL:
            case MB_FUNC_WRITE_MULTIPLE_COILS:
                _mb_rtu_m_coils_cb(rtu_master->rx_buf, address, nRegs, MB_REG_WRITE);break;
            
            case MB_FUNC_READ_INPUT_REGISTER:
                _mb_rtu_m_input_cb(rtu_master->rx_buf, address, nRegs);break;
            case MB_FUNC_WRITE_REGISTER:
            case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
            case MB_FUNC_READWRITE_MULTIPLE_REGISTERS:
                _mb_rtu_m_holding_register_cb(rtu_master->rx_buf, address, nRegs, MB_REG_WRITE);break;
            case MB_FUNC_READ_HOLDING_REGISTER:
                _mb_rtu_m_holding_register_cb(rtu_master->rx_buf, address, nRegs, MB_REG_READ);break;
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
    int16_t crc = _mb_crc(data, 6);
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
    if( len > MB_MAX_REG_LEN || len < 0 ) return MB_ERR_REG_LEN;
    if( len * 2 > MB_MAX_RTU_MESSAGE_LENGTH - 9) return MB_ERR_MAX_RTU_MSG_LEN;

    rtu_master->id = id;
    rtu_master->tx_buf[0] = id;
    rtu_master->tx_buf[1] = MB_FUNC_WRITE_REGISTER;
    rtu_master->tx_buf[2] = address >> 8;
    rtu_master->tx_buf[3] = address;
    rtu_master->tx_buf[4] = value >> 8;
    rtu_master->tx_buf[5] = value;
    rtu_master->tx_buf[6] = len * 2;
    int16_t crc = _mb_crc(data, 7);
    data[7] = crc >> 8;
    data[8] = crc;

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
    rtu_master->tx_buf[1] = MB_WRITE_MULTIPLE_REGISTERS;
    rtu_master->tx_buf[2] = address >> 8;
    rtu_master->tx_buf[3] = address;
    rtu_master->tx_buf[4] = len >> 8;
    rtu_master->tx_buf[5] = len;
    rtu_master->tx_buf[6] = len * 2;
    for( int i = 0; i < len; i++){
        rtu_master->tx_buf[7 + (2 * i)] = values[i] >> 8;
        rtu_master->tx_buf[8 + (2 * i)] = values[i];
    }
    int16_t crc = _mb_crc(data, 7 + len * 2);
    data[7 + len * 2] = crc >> 8;
    data[8 + len * 2] = crc;

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
static evm_val_t evm_module_modbus_rtu(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct device * dev = (struct device *)nevm_object_get_ext_data(p);

    struct uart_config uart_cfg = {
        .baudrate = evm_2_integer(v),
        .data_bits = UART_CFG_DATA_BITS_8,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };
    uart_configure(dev, &uart_cfg);
    mb_rtu_master_t * rtu_master = evm_malloc(sizeof(mb_rtu_master_t));
    rtu_master->dev = dev;
    uart_handle->obj = *p;
    uart_irq_callback_user_data_set(dev, (uart_irq_callback_user_data_t)uart_irq_handler, rtu_master);
    uart_irq_rx_enable(dev);

    struct k_timer * timer = (struct k_timer*)evm_malloc(sizeof(struct k_timer));
    if( !timer ) {
		evm_set_err(e, ec_type, "Can't create timer");
		return EVM_VAL_UNDEFINED;
	}

	k_timer_init(dev, _mb_rtu_m_timer_cb, NULL);

    rtu_master->timer = timer;
    timer->user_data = rtu_master;

	return EVM_VAL_UNDEFINED;
}

static evm_val_t evm_module_modbus_rtu_read_holding_reg(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);

	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_ADCDeivce_read, 0, NULL);
}

static evm_val_t evm_module_modbus_rtu_write_holding_reg(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);

	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_ADCDeivce_read, 0, NULL);
}


static evm_val_t evm_module_modbus_rtu_write_multiple_holding_reg(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	EVM_UNUSED(e);
	EVM_UNUSED(argc);
	EVM_UNUSED(v);

	evm_val_t dev = evm_mk_object((void *)nevm_object_get_ext_data(p));
	return nevm_object_function_invoke(nevm_runtime, &dev, EXPORT_ADCDeivce_read, 0, NULL);
}

evm_val_t evm_class_modbus_rtu(evm_t *e)
{
	evm_builtin_t class_adc[] = {
		{"ModbusRTUMaster", evm_mk_native((intptr_t)evm_module_adc_read)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_module_adc, class_adc, NULL);
}


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
#define DT_DRV_COMPAT rs485

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

#define MB_TX_RX_DELAY_MS           10
#define MB_DEFAULT_TIMEOUT_MS       500

typedef enum
{
    MB_REG_READ,
    MB_REG_WRITE              
} MBRegisterMode;

typedef struct mb_rtu_master_t {
    struct k_timer * timer;
    struct k_timer * timeout;
    uint32_t timeout_period;
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

    struct device *tr_gpio;
    int tr_pin;
} mb_rtu_master_t;

static const uint8_t aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const uint8_t aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};


uint16_t _mb_crc( uint8_t * pucFrame, uint16_t usLen )
{
    uint8_t           ucCRCHi = 0xFF;
    uint8_t           ucCRCLo = 0xFF;
    int               iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( uint8_t )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
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
    k_timer_stop(rtu_master->timeout);
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
    rtu_master->tx_buf[6] = crc;
    rtu_master->tx_buf[7] = crc >> 8;
    if( rtu_master->tr_gpio ) gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 1);
    for (int i = 0; i < 8; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    if( rtu_master->tr_gpio ) {
        k_sleep(K_MSEC(MB_TX_RX_DELAY_MS));
        gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 0);
    }

    k_timer_start(rtu_master->timeout, K_MSEC(rtu_master->timeout_period), K_NO_WAIT);
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
    rtu_master->tx_buf[6] = crc;
    rtu_master->tx_buf[7] = crc >> 8;
    if( rtu_master->tr_gpio )  gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 1);
    for (int i = 0; i < 8; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    if( rtu_master->tr_gpio ) {
        k_sleep(K_MSEC(MB_TX_RX_DELAY_MS));
        gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 0);
    }

    k_timer_start(rtu_master->timeout, K_MSEC(rtu_master->timeout_period), K_NO_WAIT);
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
    rtu_master->tx_buf[7 + len * 2] = crc;
    rtu_master->tx_buf[8 + len * 2] = crc >> 8;

    int data_len = 9 + len * 2;
    if( rtu_master->tr_gpio ) gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 1);
    for (int i = 0; i < data_len; i++) uart_poll_out(rtu_master->dev, rtu_master->tx_buf[i]);
    if( rtu_master->tr_gpio ) {
        k_sleep(K_MSEC(MB_TX_RX_DELAY_MS));
        gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 0);
    }

    k_timer_start(rtu_master->timeout, K_MSEC(rtu_master->timeout_period), K_NO_WAIT);
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
/**
 * destroy function will be triggered when object is collected by gc
 */
static evm_val_t evm_module_modbus_rtu_master_destroy(evm_t *e, evm_val_t *p, int argc, evm_val_t *v){
    mb_rtu_master_t * rtu_master = (mb_rtu_master_t * )evm_object_get_ext_data(p);
    if( !rtu_master ) return EVM_VAL_UNDEFINED;
    if( !rtu_master->timer ) {
        k_timer_stop(rtu_master->timer);
        evm_free(rtu_master->timer);
    }

    if( !rtu_master->timeout ) {
        k_timer_stop(rtu_master->timeout);
        evm_free(rtu_master->timeout);
    }

    evm_remove_reference(rtu_master->obj);
    evm_remove_reference(rtu_master->reg_callback);
    evm_remove_reference(rtu_master->input_callback);
    evm_remove_reference(rtu_master->discrete_callback);
    evm_remove_reference(rtu_master->coils_callback);

    evm_free(rtu_master->tx_buf);
    evm_free(rtu_master->rx_buf);

    return EVM_VAL_UNDEFINED;
}

//ModbusRTUMaster(devName, baudrate, tr_port, tr_pin)
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

        if( argc == 4 && evm_is_string(v + 2) && evm_is_number(v + 3) ){
            rtu_master->tr_gpio = device_get_binding(evm_2_string(v + 2));
            if (!rtu_master->tr_gpio) {
                evm_set_err(e, ec_type, "Can't find tx/rx pin");
                return EVM_VAL_UNDEFINED;
            }

            rtu_master->tr_pin = evm_2_integer(v + 3);

            if (gpio_pin_configure(rtu_master->tr_gpio, rtu_master->tr_pin, GPIO_OUTPUT )) {
                evm_set_err(e, ec_type, "Unable to configure tx/rx pin");
                return EVM_VAL_UNDEFINED;
            }

            gpio_pin_set(rtu_master->tr_gpio, (gpio_pin_t)rtu_master->tr_pin, 0);
        } else {
            rtu_master->tr_gpio = NULL;
        }

        rtu_master->tx_buf = evm_malloc(MB_MAX_RTU_MESSAGE_LENGTH);
        rtu_master->rx_buf = evm_malloc(MB_MAX_RTU_MESSAGE_LENGTH);
        rtu_master->rx_count = 0;

        rtu_master->timeout_period = MB_DEFAULT_TIMEOUT_MS;

        evm_object_set_destroy(p, (evm_native_fn)evm_module_modbus_rtu_master_destroy);
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
    if( argc > 2 && evm_is_number(v) && evm_is_number(v + 1) ){
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

//set_timeout(timeout_ms)
static evm_val_t evm_module_modbus_rtu_master_set_timeout(evm_t *e, evm_val_t *p, int argc, evm_val_t *v){
    if( argc >0 ){
        mb_rtu_master_t * rtu_master = (mb_rtu_master_t * )evm_object_get_ext_data(p);
        rtu_master->timeout_period = evm_2_integer(v);
    }
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_modbus_rtu_master(evm_t *e)
{
	evm_builtin_t clazz[] = {
		{"read_registers", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_read_registers)},
        {"write_registers", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_write_registers)},
        {"get_values", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_get_values)},
        {"set_timeout", evm_mk_native((intptr_t)evm_module_modbus_rtu_master_set_timeout)},
		{NULL, EVM_VAL_UNDEFINED}};
	return *evm_class_create(e, (evm_native_fn)evm_module_modbus_rtu_master, clazz, NULL);
}


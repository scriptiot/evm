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
#ifdef CONFIG_EVM_HEATSHRINK

#include "evm.h"
#include "compress_heatshrink.h"
#include "wrap_heatshrink.h"


evm_val_t wrap_heatshrink_compress(evm_t * e, evm_val_t * p, int argc, evm_val_t * v) {
  if( argc > 0 ){
    uint8_t *  in_it;
    if( evm_is_buffer(v) ) {
      in_it = evm_buffer_addr(v);
    } else if( evm_is_string(v) ) {
      in_it = evm_2_string(v);
    }

    uint32_t compressedSize = heatshrink_encode_cb(heatshrink_var_input_cb, (uint32_t*)in_it, NULL, NULL);

    evm_val_t *outVar = evm_buffer_create(e, compressedSize);
    if (!outVar) {
      return EVM_VAL_UNDEFINED;
    }
    uint8_t * out_it = evm_buffer_addr(outVar);

    heatshrink_encode_cb(heatshrink_var_input_cb, (uint32_t*)in_it, heatshrink_var_output_cb, (uint32_t*)out_it);

    return *outVar;
  } 
  return EVM_VAL_UNDEFINED;
}


evm_val_t wrap_heatshrink_decompress(evm_t * e, evm_val_t * p, int argc, evm_val_t * v) {
  if( argc > 0 ){
    uint8_t *  in_it;
    if( evm_is_buffer(v) ) {
      in_it = evm_buffer_addr(v);
    } else if( evm_is_string(v) ) {
      in_it = evm_2_string(v);
    }

    uint32_t decompressedSize = heatshrink_decode(heatshrink_var_input_cb, (uint32_t*)in_it, NULL, NULL);

    evm_val_t *outVar = evm_buffer_create(e, decompressedSize);
    if (!outVar) {
      return EVM_VAL_UNDEFINED;
    }
    uint8_t * out_it = evm_buffer_addr(outVar);

    heatshrink_decode_cb(heatshrink_var_input_cb, (uint32_t*)in_it, heatshrink_var_output_cb, (uint32_t*)out_it);

    return *outVar;
  } 
  return EVM_VAL_UNDEFINED;
}

int heatshrink_module(evm_t * e){
    evm_builtin_t module[] = {
        {"compress", evm_mk_native( (intptr_t)wrap_heatshrink_compress )},
        {"decompress", evm_mk_native( (intptr_t)wrap_heatshrink_decompress )},
        {NULL, EVM_VAL_UNDEFINED}
    };
    evm_module_create(e, "evm", module);
    return e->err;
}
#endif

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
#ifdef CONFIG_EVM_NETWORK

#include "evm_module.h" 

#include <net/net_if.h>

//network(driver_name)
static evm_val_t evm_module_network(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if( argc > 0){
        struct device * dev = device_get_binding(evm_2_string(v));
        if( !dev ){
            evm_set_err(e, ec_type, "Can't find network device");
            return EVM_VAL_UNDEFINED;
        }
        struct net_if * iface = net_if_lookup_by_dev(dev);
        if( !iface ) {
            evm_set_err(e, ec_type, "Can't find network interface");
            return EVM_VAL_UNDEFINED;
        }
        evm_object_set_ext_data(p, (intptr_t)iface);
    } else {
        EVM_ARG_LENGTH_ERR
    }
	return EVM_VAL_UNDEFINED;
}

//active('up'): activate network
//active('down') 
//active()
static evm_val_t evm_module_network_active(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct net_if * iface = (struct net_if *)evm_object_get_ext_data(p);
    if( !iface ) {
        evm_set_err(e, ec_type, "Can't find network interface");
        return EVM_VAL_UNDEFINED;
    }
    if( argc == 0 ){
        if( net_if_flag_is_set(iface, NET_IF_UP) ) return EVM_VAL_TRUE;
        else return EVM_VAL_FALSE;
    } else if(evm_is_string(v)) {
        const char * s = evm_2_string(v);
        if( !strcmp(s, "up") ){
            net_if_flag_set(iface, NET_IF_UP);
        } else if( !strcmp(s, "down") ) {
            net_if_flag_clear(iface, NET_IF_UP);
        }
    }
    return EVM_VAL_UNDEFINED;
}

//ifconfig(ip, subnet, gateway)
static evm_val_t evm_module_network_ifconfig(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
	struct net_if * iface = (struct net_if *)evm_object_get_ext_data(p);
    if( !iface ) {
        evm_set_err(e, ec_type, "Can't find network interface");
        return EVM_VAL_UNDEFINED;
    }
    struct in_addr ip_addr;
    struct in_addr mask_addr;
    struct in_addr gw_addr;
    
    if( argc > 2 && evm_is_string(v) && evm_is_string(v + 1) && evm_is_string(v + 2)){
        char * ip = (char*)evm_2_string(v);
        char * subnet = (char*)evm_2_string(v + 1);
        char * gateway = (char*)evm_2_string(v + 2);

        net_addr_pton(AF_INET, ip, &ip_addr);
        net_addr_pton(AF_INET, subnet, &mask_addr);
        net_addr_pton(AF_INET, gateway, &gw_addr);

        net_if_ipv4_addr_add(iface, &ip_addr, NET_ADDR_MANUAL, 0);
        net_if_ipv4_set_netmask(iface, &mask_addr);
        net_if_ipv4_set_gw(iface, &gw_addr);
    } else if( argc > 1 && evm_is_string(v) && evm_is_string(v + 1) ) {
        char * ip = (char*)evm_2_string(v);
        char * subnet = (char*)evm_2_string(v + 1);

        net_addr_pton(AF_INET, ip, &ip_addr);
        net_addr_pton(AF_INET, subnet, &mask_addr);

        net_if_ipv4_addr_add(iface, &ip_addr, NET_ADDR_MANUAL, 0);
        net_if_ipv4_set_netmask(iface, &mask_addr);
    } else if( argc > 0){
        char * ip = (char*)evm_2_string(v);
        net_addr_pton(AF_INET, ip, &ip_addr);
        net_if_ipv4_addr_add(iface, &ip_addr, NET_ADDR_MANUAL, 0);
    } else {
        char buf[NET_IPV4_ADDR_LEN];
        evm_val_t * result = evm_list_create(e, GC_LIST, 3);

        evm_val_t * ip_val = evm_heap_string_create(e, net_addr_ntop(AF_INET,
			    &iface->config.ip.ipv4->unicast[0].address.in_addr,
						  buf, sizeof(buf)), strlen(buf));

        evm_val_t * subnet_val = evm_heap_string_create(e, net_addr_ntop(AF_INET,
				       &iface->config.ip.ipv4->netmask,
				       buf, sizeof(buf)), strlen(buf));

        evm_val_t * gw_val = evm_heap_string_create(e, net_addr_ntop(AF_INET,
						 &iface->config.ip.ipv4->gw,
						 buf, sizeof(buf)), strlen(buf));
        evm_list_set(e, result, 0, *ip_val);
        evm_list_set(e, result, 1, *subnet_val);
        evm_list_set(e, result, 2, *gw_val);
    }

	return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_network(evm_t * e){
	evm_builtin_t class_network[] = {
		{"active", evm_mk_native( (intptr_t)evm_module_network_active )},
        {"ifconfig", evm_mk_native( (intptr_t)evm_module_network_ifconfig )},
		{NULL, EVM_VAL_UNDEFINED}
	};
	return *evm_class_create(e, (evm_native_fn)evm_module_network, class_network, NULL);
}
#endif

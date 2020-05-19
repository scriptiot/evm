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
#ifdef CONFIG_EVM_WIFI

#include "evm_module.h"

#include <errno.h>
#include <net/net_core.h>
#include <net/net_if.h>
#include <net/wifi_mgmt.h>


#include <zephyr.h>
#include <stdio.h>
#include <stdlib.h>
#include <init.h>

#include <net/net_if.h>
#include <net/wifi_mgmt.h>
#include <net/net_event.h>

#define WIFI_SHELL_MODULE "wifi"

#define WIFI_SHELL_MGMT_EVENTS (NET_EVENT_WIFI_SCAN_RESULT |		\
				NET_EVENT_WIFI_SCAN_DONE |		\
				NET_EVENT_WIFI_CONNECT_RESULT |		\
				NET_EVENT_WIFI_DISCONNECT_RESULT)

static struct {
	const struct shell *shell;

	union {
		struct {

			u8_t connecting		: 1;
			u8_t disconnecting	: 1;
			u8_t _unused		: 6;
		};
		u8_t all;
	};
} context;

static u32_t scan_result;

static struct net_mgmt_event_callback wifi_shell_mgmt_cb;

#define print(shell, level, fmt, ...)					\
	do {								\
		printk(fmt, ##__VA_ARGS__);			\
	} while (false)

static void handle_wifi_scan_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_scan_result *entry =
		(const struct wifi_scan_result *)cb->info;

	scan_result++;

	if (scan_result == 1U) {
		print(context.shell, SHELL_NORMAL,
		      "%-4s | %-32s %-5s | %-4s | %-4s | %-5s\n",
		      "Num", "SSID", "(len)", "Chan", "RSSI", "Sec");
	}

	print(context.shell, SHELL_NORMAL,
	      "%-4d | %-32s %-5u | %-4u | %-4d | %-5s\n",
	      scan_result, entry->ssid, entry->ssid_length,
	      entry->channel, entry->rssi,
	      (entry->security == WIFI_SECURITY_TYPE_PSK ?
	       "WPA/WPA2" : "Open"));
}

static void handle_wifi_scan_done(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *)cb->info;

	if (status->status) {
		print(context.shell, SHELL_WARNING,
		      "Scan request failed (%d)\n", status->status);
	} else {
		print(context.shell, SHELL_NORMAL, "Scan request done\n");
	}

	scan_result = 0U;
}

static void handle_wifi_connect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (status->status) {
		print(context.shell, SHELL_WARNING,
		      "Connection request failed (%d)\n", status->status);
	} else {
		print(context.shell, SHELL_NORMAL, "Connected\n");
	}

	context.connecting = false;
}

static void handle_wifi_disconnect_result(struct net_mgmt_event_callback *cb)
{
	const struct wifi_status *status =
		(const struct wifi_status *) cb->info;

	if (context.disconnecting) {
		print(context.shell,
		      status->status ? SHELL_WARNING : SHELL_NORMAL,
		      "Disconnection request %s (%d)\n",
		      status->status ? "failed" : "done",
		      status->status);
		context.disconnecting = false;
	} else {
		print(context.shell, SHELL_NORMAL, "Disconnected\n");
	}
}

static void wifi_mgmt_event_handler(struct net_mgmt_event_callback *cb,
				    u32_t mgmt_event, struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_WIFI_SCAN_RESULT:
		handle_wifi_scan_result(cb);
		break;
	case NET_EVENT_WIFI_SCAN_DONE:
		handle_wifi_scan_done(cb);
		break;
	case NET_EVENT_WIFI_CONNECT_RESULT:
		handle_wifi_connect_result(cb);
		break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		handle_wifi_disconnect_result(cb);
		break;
	default:
		break;
	}
}

/**
 * @brief WIFI Interface class constructor
 * 
 * @param name device name
 * 
 * @uasge new WIFI() 
 */
static evm_val_t evm_module_wifi(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    net_mgmt_init_event_callback(&wifi_shell_mgmt_cb,
				     wifi_mgmt_event_handler,
				     WIFI_SHELL_MGMT_EVENTS);
    net_mgmt_add_event_callback(&wifi_shell_mgmt_cb);
    return EVM_VAL_UNDEFINED;
}
/**
 * @brief Connect to the specified wireless network, using the specified password.
 * 
 * @uasge WIFI.connect(ssid, password, channel)
 */
static evm_val_t evm_module_wifi_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct net_if *iface = net_if_get_default();
	static struct wifi_connect_req_params cnx_params;
    
    if (argc < 1)  return EVM_VAL_FALSE;
    if( !evm_is_string(v) ) return EVM_VAL_FALSE;

    cnx_params.ssid = evm_2_string(v);
	cnx_params.ssid_length = strlen(cnx_params.ssid);

    if( argc == 1 ) {
        cnx_params.security = WIFI_SECURITY_TYPE_NONE;
    } else if( argc == 2 ) {
        if( evm_is_integer(v + 1) ) { 
            cnx_params.channel = evm_2_integer(v + 1);
        } else if( evm_is_string(v + 1) ) {
            cnx_params.channel = WIFI_CHANNEL_ANY;
            cnx_params.psk = evm_2_string(v + 1);
            cnx_params.psk_length = strlen(cnx_params.psk);
            cnx_params.security = WIFI_SECURITY_TYPE_PSK;
        } else 
            return EVM_VAL_FALSE;
    } else {
        if( evm_is_string(v + 1) && evm_is_integer(v + 2) ) {
            cnx_params.psk = evm_2_string(v + 1);
            cnx_params.psk_length = strlen(cnx_params.psk);
            cnx_params.security = WIFI_SECURITY_TYPE_PSK;

            cnx_params.channel = evm_2_integer(v + 2);
        } else
            return EVM_VAL_FALSE;
    }

	if (net_mgmt(NET_REQUEST_WIFI_CONNECT, iface, &cnx_params, sizeof(struct wifi_connect_req_params))) {
		evm_print("Connection request failed\r\n");
        return EVM_VAL_FALSE;
	} else {
        evm_print("Connection request\r\n");
		return EVM_VAL_TRUE;
	}
}

/**
 * @brief disconnect wifi
 * 
 * @uasge WIFI.disconnect()
 */
static evm_val_t evm_module_wifi_disconnect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct net_if *iface = net_if_get_default();
	int status;

	status = net_mgmt(NET_REQUEST_WIFI_DISCONNECT, iface, NULL, 0);

	if (status) {
		if (status == -EALREADY) {
			evm_print("Already disconnected\r\n");
		} else {
			evm_print("Disconnect request failed\r\n");
		}
        return EVM_VAL_FALSE;
	} else {
		evm_print("Disconnect requested\r\n");
        return EVM_VAL_TRUE;
	}
}

/**
 * @brief read adc value
 * 
 * @uasge WIFI.scan()
 */
static evm_val_t evm_module_wifi_scan(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    struct net_if *iface = net_if_get_default();

	if (net_mgmt(NET_REQUEST_WIFI_SCAN, iface, NULL, 0)) {
		evm_print("Scan request failed\r\n");
	} else {
		evm_print( "Scan requested\r\n");
	}
    return EVM_VAL_UNDEFINED;
}

evm_val_t evm_class_wifi(evm_t *e)
{
	evm_builtin_t class_wifi[] = {
		{"connect", evm_mk_native((intptr_t)evm_module_wifi_connect)},
        {"disconnect", evm_mk_native((intptr_t)evm_module_wifi_disconnect)},
        {"scan", evm_mk_native((intptr_t)evm_module_wifi_scan)},
		{NULL, NULL}};
	return *evm_class_create(e, (evm_native_fn)evm_module_wifi, class_wifi, NULL);
}
#endif

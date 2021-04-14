#ifdef CONFIG_EVM_MODULE_WIFI
#include "evm_module.h"
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <bl_wifi.h>
#include <hal_wifi.h>
#include <easyflash.h>
#include <wifi_mgmr_ext.h>

#define WIFI_AP_PSM_INFO_SSID "bytecode"
#define WIFI_AP_PSM_INFO_PASSWORD "bytecode888"
#define WIFI_AP_PSM_INFO_PMK "conf_ap_pmk"
#define WIFI_AP_PSM_INFO_BSSID "conf_ap_bssid"
#define WIFI_AP_PSM_INFO_CHANNEL "conf_ap_channel"

static wifi_interface_t wifi_interface;
static wifi_conf_t conf = {
    .country_code = "CN",
};

static unsigned char char_to_hex(char asccode)
{
    unsigned char ret;

    if ('0' <= asccode && asccode <= '9')
        ret = asccode - '0';
    else if ('a' <= asccode && asccode <= 'f')
        ret = asccode - 'a' + 10;
    else if ('A' <= asccode && asccode <= 'F')
        ret = asccode - 'A' + 10;
    else
        ret = 0;

    return ret;
}

static void _chan_str_to_hex(uint8_t *chan_band, uint16_t *chan_freq, char *chan)
{
    int i, freq_len, base = 1;
    uint8_t band;
    uint16_t freq = 0;
    char *p, *q;

    /*should have the following format
     * 2412|0
     * */
    p = strchr(chan, '|') + 1;
    if (NULL == p)
    {
        return;
    }
    band = char_to_hex(p[0]);
    (*chan_band) = band;

    freq_len = strlen(chan) - strlen(p) - 1;
    q = chan;
    q[freq_len] = '\0';
    for (i = 0; i < freq_len; i++)
    {
        freq = freq + char_to_hex(q[freq_len - 1 - i]) * base;
        base = base * 10;
    }
    (*chan_freq) = freq;
}

static void bssid_str_to_mac(uint8_t *hex, char *bssid, int len)
{
    unsigned char l4, h4;
    int i, lenstr;
    lenstr = len;

    if (lenstr % 2)
    {
        lenstr -= (lenstr % 2);
    }

    if (lenstr == 0)
    {
        return;
    }

    for (i = 0; i < lenstr; i += 2)
    {
        h4 = char_to_hex(bssid[i]);
        l4 = char_to_hex(bssid[i + 1]);
        hex[i / 2] = (h4 << 4) + l4;
    }
}

int check_dts_config(char ssid[33], char password[64])
{
    bl_wifi_ap_info_t sta_info;

    if (bl_wifi_sta_info_get(&sta_info))
    {
        /*no valid sta info is got*/
        return -1;
    }

    strncpy(ssid, (const char *)sta_info.ssid, 32);
    ssid[31] = '\0';
    strncpy(password, (const char *)sta_info.psk, 64);
    password[63] = '\0';

    return 0;
}

static void _connect_wifi()
{
    /*XXX caution for BIG STACK*/
    char pmk[66], bssid[32], chan[10];
    char ssid[33], password[66];
    char val_buf[66];
    char val_len = sizeof(val_buf) - 1;
    uint8_t mac[6];
    uint8_t band = 0;
    uint16_t freq = 0;

    wifi_interface = wifi_mgmr_sta_enable();
    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]   Get STA %p from Wi-Fi Mgmr, pmk ptr %p, ssid ptr %p, password %p\r\n",
           aos_now_ms(),
           wifi_interface,
           pmk,
           ssid,
           password);
    memset(pmk, 0, sizeof(pmk));
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    memset(bssid, 0, sizeof(bssid));
    memset(mac, 0, sizeof(mac));
    memset(chan, 0, sizeof(chan));
    memset(val_buf, 0, sizeof(val_buf));

    printf("$$$$$$$$$$$$$$$$$$ %s $$$$$$$$$$$$$$$$$$$", val_buf[0]);

    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_SSID, val_buf, val_len, NULL);
    if (val_buf[0])
    {
        /*We believe that when ssid is set, wifi_confi is OK*/
        strncpy(ssid, val_buf, sizeof(ssid) - 1);

        /*setup password ans PMK stuff from ENV*/
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PASSWORD, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(password, val_buf, sizeof(password) - 1);
        }

        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PMK, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(pmk, val_buf, sizeof(pmk) - 1);
        }
        if (0 == pmk[0])
        {
            printf("[APP] [WIFI] [T] %lld\r\n", aos_now_ms());
            puts("[APP]    Re-cal pmk\r\n");
            /*At lease pmk is not illegal, we re-cal now*/
            //XXX time consuming API, so consider lower-prirotiy for cal PSK to avoid sound glitch
            wifi_mgmr_psk_cal(
                password,
                ssid,
                strlen(ssid),
                pmk);
            ef_set_env(WIFI_AP_PSM_INFO_PMK, pmk);
            ef_save_env();
        }
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_CHANNEL, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(chan, val_buf, sizeof(chan) - 1);
            printf("connect wifi channel = %s\r\n", chan);
            _chan_str_to_hex(&band, &freq, chan);
        }
        memset(val_buf, 0, sizeof(val_buf));
        ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_BSSID, val_buf, val_len, NULL);
        if (val_buf[0])
        {
            strncpy(bssid, val_buf, sizeof(bssid) - 1);
            printf("connect wifi bssid = %s\r\n", bssid);
            bssid_str_to_mac(mac, bssid, strlen(bssid));
            printf("mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                   mac[0],
                   mac[1],
                   mac[2],
                   mac[3],
                   mac[4],
                   mac[5]);
        }

        wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
    }
    else if (0 == check_dts_config(ssid, password))
    {
        /*nothing here*/
    }
    else
    {
        /*Won't connect, since ssid config is empty*/
        puts("[APP]    Empty Config\r\n");
        puts("[APP]    Try to set the following ENV with psm_set command, then reboot\r\n");
        puts("[APP]    NOTE: " WIFI_AP_PSM_INFO_PMK " MUST be psm_unset when conf is changed\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_SSID "\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_PASSWORD "\r\n");
        puts("[APP]    env(optinal): " WIFI_AP_PSM_INFO_PASSWORD "\r\n");
        return;
    }

    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]    SSID %s\r\n"
           "[APP]    SSID len %d\r\n"
           "[APP]    password %s\r\n"
           "[APP]    password len %d\r\n"
           "[APP]    pmk %s\r\n"
           "[APP]    bssid %s\r\n"
           "[APP]    channel band %d\r\n"
           "[APP]    channel freq %d\r\n",
           aos_now_ms(),
           ssid,
           strlen(ssid),
           password,
           strlen(password),
           pmk,
           bssid,
           band,
           freq);
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}

static void wifi_sta_connect(char *ssid, char *password)
{
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}

static void event_cb_wifi_event(input_event_t *event, void *private_data)
{
    static char *ssid;
    static char *password;

    switch (event->code)
    {
    case CODE_WIFI_ON_INIT_DONE:
    {
        printf("[APP] [EVT] INIT DONE %lld\r\n", aos_now_ms());
        wifi_mgmr_start_background(&conf);
    }
    break;
    case CODE_WIFI_ON_MGMR_DONE:
    {
        printf("[APP] [EVT] MGMR DONE %lld\r\n", aos_now_ms());
        _connect_wifi();
    }
    break;
    case CODE_WIFI_ON_MGMR_DENOISE:
    {
        printf("[APP] [EVT] Microwave Denoise is ON %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE:
    {
        printf("[APP] [EVT] SCAN Done %lld\r\n", aos_now_ms());
        wifi_mgmr_cli_scanlist();
    }
    break;
    case CODE_WIFI_ON_SCAN_DONE_ONJOIN:
    {
        printf("[APP] [EVT] SCAN On Join %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_DISCONNECT:
    {
        printf("[APP] [EVT] disconnect %lld, Reason: %s\r\n",
               aos_now_ms(),
               wifi_mgmr_status_code_str(event->value));
    }
    break;
    case CODE_WIFI_ON_CONNECTING:
    {
        printf("[APP] [EVT] Connecting %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_CMD_RECONNECT:
    {
        printf("[APP] [EVT] Reconnect %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_CONNECTED:
    {
        printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_PRE_GOT_IP:
    {
        printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_GOT_IP:
    {
        printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
        printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
    }
    break;
    case CODE_WIFI_ON_EMERGENCY_MAC:
    {
        printf("[APP] [EVT] EMERGENCY MAC %lld\r\n", aos_now_ms());
        hal_reboot(); //one way of handling emergency is reboot. Maybe we should also consider solutions
    }
    break;
    case CODE_WIFI_ON_PROV_SSID:
    {
        printf("[APP] [EVT] [PROV] [SSID] %lld: %s\r\n",
               aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (ssid)
        {
            vPortFree(ssid);
            ssid = NULL;
        }
        ssid = (char *)event->value;
    }
    break;
    case CODE_WIFI_ON_PROV_BSSID:
    {
        printf("[APP] [EVT] [PROV] [BSSID] %lld: %s\r\n",
               aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (event->value)
        {
            vPortFree((void *)event->value);
        }
    }
    break;
    case CODE_WIFI_ON_PROV_PASSWD:
    {
        printf("[APP] [EVT] [PROV] [PASSWD] %lld: %s\r\n", aos_now_ms(),
               event->value ? (const char *)event->value : "UNKNOWN");
        if (password)
        {
            vPortFree(password);
            password = NULL;
        }
        password = (char *)event->value;
    }
    break;
    case CODE_WIFI_ON_PROV_CONNECT:
    {
        printf("[APP] [EVT] [PROV] [CONNECT] %lld\r\n", aos_now_ms());
        printf("connecting to %s:%s...\r\n", ssid, password);
        wifi_sta_connect(ssid, password);
    }
    break;
    case CODE_WIFI_ON_PROV_DISCONNECT:
    {
        printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
    }
    break;
    case CODE_WIFI_ON_AP_STA_ADD:
    {
        printf("[APP] [EVT] [AP] [ADD] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
    }
    break;
    case CODE_WIFI_ON_AP_STA_DEL:
    {
        printf("[APP] [EVT] [AP] [DEL] %lld, sta idx is %lu\r\n", aos_now_ms(), (uint32_t)event->value);
    }
    break;
    default:
    {
        printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
        /*nothing*/
    }
    }
}

//sta.scan([callback])
static evm_val_t evm_module_wifi_sta_scan(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_string(v))
    {
        return EVM_VAL_UNDEFINED;
    }

    // int i = 0;
    // int length = sizeof(wifiMgmr.scan_items) / sizeof(wifiMgmr.scan_items[0]);

    // evm_val_t *list = evm_list_create(e, GC_LIST, length);
    // evm_val_t *obj;
    // for (i = 0; i < length; i++)
    // {
    //     if (wifiMgmr.scan_items[i].is_used && (!wifi_mgmr_scan_item_is_timeout(&wifiMgmr, &wifiMgmr.scan_items[i])))
    //     {
    //         obj = evm_object_create(e, GC_OBJECT, 3, 0);
    //         char *s = evm_malloc(sizeof(char) * wifiMgmr.scan_items[i].ssid_len);
    //         memcpy(s, wifiMgmr.scan_items[i].ssid, wifiMgmr.scan_items[i].ssid_len);
    //         evm_prop_append(e, obj, "ssid", evm_mk_heap_string((intptr_t)s));
    //         s = evm_malloc(sizeof(char) * 17);
    //         sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", wifiMgmr.scan_items[i].bssid[0], wifiMgmr.scan_items[i].bssid[1], wifiMgmr.scan_items[i].bssid[2],
    //                 wifiMgmr.scan_items[i].bssid[3], wifiMgmr.scan_items[i].bssid[4], wifiMgmr.scan_items[i].bssid[5]);
    //         evm_prop_append(e, obj, "bssid", evm_mk_heap_string((intptr_t)s));
    //         evm_prop_append(e, obj, "channel", evm_mk_number(wifiMgmr.scan_items[i].channel));
    //         evm_list_set(e, list, i, *obj);
    //     }
    // }

    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, &e->scope, v + 1, NULL, 0);

    // return *list;
    return EVM_VAL_UNDEFINED;
}

//sta.connect(options[, callback])
static evm_val_t evm_module_wifi_sta_connect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t *ssid = evm_prop_get(e, v, "ssid", 0);
    if (ssid == NULL)
        return EVM_VAL_UNDEFINED;

    evm_val_t *password = evm_prop_get(e, v, "password", 0);
    if (password == NULL)
        return EVM_VAL_UNDEFINED;

    int result = wifi_mgmr_sta_connect(wifi_interface, evm_2_string(ssid), evm_2_string(password), NULL, NULL, 0, 0);

    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, &e->scope, v + 1, evm_mk_number(result), 1);

    return EVM_VAL_UNDEFINED;
}

//sta.disconnect([callback])
static evm_val_t evm_module_wifi_sta_disconnect(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    wifi_mgmr_sta_disconnect();

    if (argc > 0 || !evm_is_script(v))
    {
        evm_run_callback(e, &e->scope, v, NULL, 0);
    }

    return EVM_VAL_UNDEFINED;
}

//ap.start(options[, callback])
static evm_val_t evm_module_wifi_ap_start(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t *ssid = evm_prop_get(e, v, "ssid", 0);
    if (ssid == NULL)
        return EVM_VAL_UNDEFINED;

    evm_val_t *password = evm_prop_get(e, v, "password", 0);
    if (password == NULL)
        return EVM_VAL_UNDEFINED;

    wifi_mgmr_ap_start(wifi_interface, evm_2_string(ssid), 0, evm_2_string(password), 0);

    if (argc > 0 || !evm_is_script(v + 1))
    {
        evm_run_callback(e, &e->scope, v, NULL, 0);
    }

    return EVM_VAL_UNDEFINED;
}

//ap.stop([callback])
static evm_val_t evm_module_wifi_ap_stop(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    wifi_mgmr_ap_stop(wifi_interface);

    if (argc > 0 || !evm_is_script(v))
    {
        evm_run_callback(e, &e->scope, v, NULL, 0);
    }

    return EVM_VAL_UNDEFINED;
}

static void cmd_stack_wifi(char *buf, int len, int argc, char **argv)
{
    /*wifi fw stack and thread stuff*/
    static uint8_t stack_wifi_init = 0;

    if (1 == stack_wifi_init)
    {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;

    hal_wifi_start_firmware_task();
    /*Trigger to start Wi-Fi*/
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);
}

//wifi.init([options])
static evm_val_t evm_module_wifi_init(evm_t *e, evm_val_t *p, int argc, evm_val_t *v)
{
    if (argc < 1 || !evm_is_object(v))
        return EVM_VAL_UNDEFINED;

    evm_val_t *mode = evm_prop_get(e, v, "mode", 0);
    if (mode == NULL)
        return EVM_VAL_UNDEFINED;

    evm_val_t *obj;
    if (evm_2_integer(mode) == 1)
    {
        obj = evm_object_create(e, GC_DICT, 3, 0);
        evm_prop_append(e, obj, "scan", evm_mk_native((intptr_t)evm_module_wifi_sta_scan));
        evm_prop_append(e, obj, "connect", evm_mk_native((intptr_t)evm_module_wifi_sta_connect));
        evm_prop_append(e, obj, "discpnnect", evm_mk_native((intptr_t)evm_module_wifi_sta_disconnect));
    }
    else if (evm_2_integer(mode) == 2)
    {
        obj = evm_object_create(e, GC_DICT, 2, 0);
        evm_prop_append(e, obj, "start", evm_mk_native((intptr_t)evm_module_wifi_ap_start));
        evm_prop_append(e, obj, "stop", evm_mk_native((intptr_t)evm_module_wifi_ap_stop));
    }
    else
    {
        evm_set_err(e, ec_type, "Undefine WiFi mode");
        return EVM_VAL_UNDEFINED;
    }

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);
    cmd_stack_wifi(NULL, 0, 0, NULL);

    if (argc > 1 && evm_is_script(v + 1))
        evm_run_callback(e, &e->scope, v + 1, NULL, 0);

    return *obj;
}

evm_err_t evm_module_wifi(evm_t *e)
{
    evm_builtin_t builtin[] = {
        {"init", evm_mk_native((intptr_t)evm_module_wifi_init)},
        {"STA", evm_mk_number(1)},
        {"AP", evm_mk_number(2)},
        {NULL, EVM_VAL_UNDEFINED}};
    evm_module_create(e, "wifi", builtin);
    return e->err;
}

#endif

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

#define DT_DRV_COMPAT wiznet_w5500

#define LOG_MODULE_NAME wiznet_w5500
#define LOG_LEVEL CONFIG_ETHERNET_LOG_LEVEL

#include <zephyr.h>
#include <device.h>
#include <string.h>
#include <errno.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <net/net_pkt.h>
#include <net/net_if.h>
#include <net/ethernet.h>

#include "W5500/w5500.h"
#include "wizchip_conf.h"
#include "driver_w5500_priv.h"

#define W5500_SN	0

LOG_MODULE_REGISTER(ethdrv, CONFIG_ETHERNET_LOG_LEVEL);

static struct device *wiz_dev;

static void spi_write_byte(uint8_t data)
{
    struct w5500_runtime *context = wiz_dev->driver_data;
	u8_t buf[2] = { data, 0xFF };
	const struct spi_buf tx_buf = {
		.buf = buf,
		.len = 1,
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};

	spi_write(context->spi, &context->spi_cfg, &tx);
}

static uint8_t spi_read_byte()
{
    struct w5500_runtime *context = wiz_dev->driver_data;
	u8_t buf[1];
	const struct spi_buf tx_buf = {
		.buf = buf,
		.len = 1
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = 1
	};
	struct spi_buf rx_buf = {
		.buf = buf,
		.len = sizeof(buf),
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = 1
	};

	buf[0] = 0x00;

	if (!spi_transceive(context->spi, &context->spi_cfg, &tx, &rx)) {
		return buf[0];
	} else {
		LOG_DBG("Failed to read byte");
		return 0;
	}
}

static void spi_write_burst(uint8_t *buf, uint16_t len)
{
    struct w5500_runtime *context = wiz_dev->driver_data;
	const struct spi_buf tx_buf = {
		.buf = buf,
		.len = len,
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = len
	};

	spi_write(context->spi, &context->spi_cfg, &tx);
}

static void spi_read_burst(uint8_t *buf, uint16_t len)
{
    struct w5500_runtime *context = wiz_dev->driver_data;
	const struct spi_buf tx_buf = {
		.buf = buf,
		.len = len
	};
	const struct spi_buf_set tx = {
		.buffers = &tx_buf,
		.count = len
	};
	struct spi_buf rx_buf = {
		.buf = buf,
		.len = len,
	};
	const struct spi_buf_set rx = {
		.buffers = &rx_buf,
		.count = len
	};

	if (spi_transceive(context->spi, &context->spi_cfg, &tx, &rx)) {
		LOG_DBG("Fail to read burst");
	} 
}

static void spi_cris_enter(void)
{
	struct w5500_runtime *context = wiz_dev->driver_data;
	const struct w5500_config  *config = wiz_dev->config_info;

	z_impl_gpio_disable_callback(context->gpio,config->gpio_pin);
}

static void spi_cris_exit(void)
{
	struct w5500_runtime *context = wiz_dev->driver_data;
	const struct w5500_config  *config = wiz_dev->config_info;

	z_impl_gpio_enable_callback(context->gpio,config->gpio_pin);
}

static void spi_cs_select(void)
{
	struct w5500_runtime *context = wiz_dev->driver_data;
    gpio_pin_set(context->spi_cs.gpio_dev,context->spi_cs.gpio_pin,0);
}

static void spi_cs_deselect(void)
{
	struct w5500_runtime *context = wiz_dev->driver_data;

    gpio_pin_set(context->spi_cs.gpio_dev,context->spi_cs.gpio_pin,0);
}

/* register TCP communication related callback function */
static int wiz_callback_register(void)
{
    /* register critical section callback function */
    reg_wizchip_cris_cbfunc(spi_cris_enter, spi_cris_exit);

#if (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_) || (_WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_)
    /* register SPI device CS select callback function */
    reg_wizchip_cs_cbfunc(spi_cs_select, spi_cs_deselect);
#else
#if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
#error "Unknown _WIZCHIP_IO_MODE_"
#else
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#endif
#endif
    /* register SPI device read/write data callback function */
    reg_wizchip_spi_cbfunc(spi_read_byte, spi_write_byte);
    reg_wizchip_spiburst_cbfunc(spi_read_burst, spi_write_burst);

    return 1;
}

static void w5500_gpio_callback(struct device *dev,
				       struct gpio_callback *cb,
				       u32_t pins)
{
	struct w5500_runtime *context =
		CONTAINER_OF(cb, struct w5500_runtime, gpio_cb);

	k_sem_give(&context->int_sem); 
}

static int w5500_tx(struct device *dev, struct net_pkt *pkt)
{
	struct w5500_runtime *context = dev->driver_data;
	u16_t len = net_pkt_get_len(pkt);
	struct net_buf *frag;

	LOG_DBG("pkt %p (len %u)", pkt, len);

	k_sem_take(&context->tx_rx_sem, K_FOREVER);

	for (frag = pkt->frags; frag; frag = frag->frags) {
		wiz_send_data(W5500_SN, frag->data, frag->len);
	}

	k_sem_give(&context->tx_rx_sem);

	return 0;
}

static int w5500_rx(struct device *dev)
{
	struct w5500_runtime *context = dev->driver_data;
	const struct w5500_config *config = dev->config_info;
	struct net_buf *pkt_buf = NULL;
	struct net_pkt *pkt;
	u16_t frm_len = 0U;

	frm_len = getSn_RX_RSR(W5500_SN);
	if( frm_len < 4 ) goto done;
	wiz_recv_data(W5500_SN, (uint8_t *)&frm_len, 2);
	frm_len = (frm_len << 8) | (frm_len >> 8);
	if (frm_len > NET_ETH_MAX_FRAME_SIZE) {
		LOG_ERR("Maximum frame length exceeded");
		wiz_recv_ignore(W5500_SN, frm_len);
    	setSn_CR(W5500_SN, Sn_CR_RECV);
		goto done;
	}
	frm_len-= 2;
	/* Get the frame from the buffer */
	pkt = net_pkt_rx_alloc_with_buffer(context->iface, frm_len,
					   AF_UNSPEC, 0,
					   K_MSEC(config->timeout));
	if (!pkt) {
		LOG_ERR("Could not allocate rx buffer");
		goto done;
	}

	pkt_buf = pkt->buffer;

	do {
		size_t frag_len;
		u8_t *data_ptr;
		size_t spi_frame_len;

		data_ptr = pkt_buf->data;

		/* Review the space available for the new frag */
		frag_len = net_buf_tailroom(pkt_buf);

		if (frm_len > frag_len) {
			spi_frame_len = frag_len;
		} else {
			spi_frame_len = frm_len;
		}

		wiz_recv_data(0, data_ptr, spi_frame_len);
  		setSn_CR(0, Sn_CR_RECV);

		net_buf_add(pkt_buf, spi_frame_len);

		/* One fragment has been written via SPI */
		frm_len -= spi_frame_len;
		pkt_buf = pkt_buf->frags;
	} while (frm_len > 0);

	if (net_recv_data(context->iface, pkt) < 0) {
		net_pkt_unref(pkt);
	}

done:
	k_sem_give(&context->tx_rx_sem);
	return 0;
}

static void w5500_rx_thread(struct device *dev)
{
	struct w5500_runtime *context = dev->driver_data;

	while (true) {
		k_sem_take(&context->int_sem, K_FOREVER);
		w5500_rx(dev);
	}
}

static enum ethernet_hw_caps w5500_get_capabilities(struct device *dev)
{
	ARG_UNUSED(dev);
	return ETHERNET_LINK_10BASE_T | ETHERNET_LINK_100BASE_T;
}

static void w5500_init_phy(struct device *dev)
{
	const struct w5500_config *config = dev->config_info;

	uint8_t tmp = 0;

	if (config->full_duplex) {
		tmp |= PHYCFGR_OPMDC_100F;
	} else {
		tmp |= PHYCFGR_OPMDC_100H;
	}
	setPHYCFGR(tmp);
}

static void w5500_iface_init(struct net_if *iface)
{
	struct device *dev = net_if_get_device(iface);
	struct w5500_runtime *context = dev->driver_data;

	net_if_set_link_addr(iface, context->mac_address,
			     sizeof(context->mac_address),
			     NET_LINK_ETHERNET);
	context->iface = iface;
	ethernet_init(iface);

	net_if_flag_set(iface, NET_IF_NO_AUTO_START);
	context->iface_initialized = true;
}

static const struct ethernet_api api_funcs = {
	.iface_api.init		= w5500_iface_init,

	.get_capabilities	= w5500_get_capabilities,
	.send			= w5500_tx,
};

static int w5500_init(struct device *dev)
{
	wiz_dev = dev;
	
	const struct w5500_config *config = dev->config_info;
	struct w5500_runtime *context = dev->driver_data;

	/* SPI config */
	context->spi_cfg.operation = SPI_WORD_SET(8);
	context->spi_cfg.frequency = config->spi_freq;
	context->spi_cfg.slave = config->spi_slave;

	context->spi = device_get_binding((char *)config->spi_port);
	if (!context->spi) {
		LOG_ERR("SPI master port %s not found", config->spi_port);
		return -EINVAL;
	}

#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	context->spi_cs.gpio_dev =
		device_get_binding((char *)config->spi_cs_port);
	if (!context->spi_cs.gpio_dev) {
		LOG_ERR("SPI CS port %s not found", config->spi_cs_port);
		return -EINVAL;
	}

	context->spi_cs.gpio_pin = config->spi_cs_pin;
	context->spi_cfg.cs = &context->spi_cs;
#endif

/* Initialize GPIO */
	context->gpio = device_get_binding((char *)config->gpio_port);
	if (!context->gpio) {
		LOG_ERR("GPIO port %s not found", config->gpio_port);
		return -EINVAL;
	}

	if (gpio_pin_configure(context->gpio, config->gpio_pin,
			       GPIO_INPUT | config->gpio_flags)) {
		LOG_ERR("Unable to configure GPIO pin %u", config->gpio_pin);
		return -EINVAL;
	}

	gpio_init_callback(&(context->gpio_cb), w5500_gpio_callback,
			   BIT(config->gpio_pin));

	if (gpio_add_callback(context->gpio, &(context->gpio_cb))) {
		return -EINVAL;
	}

	gpio_pin_interrupt_configure(context->gpio,
				     config->gpio_pin,
				     GPIO_INT_EDGE_TO_ACTIVE);
					 
	wiz_callback_register();
	/* read MAC address */
	getPHAR(context->mac_address);
	w5500_init_phy(dev);

	/* Start interruption-poll thread */
	k_thread_create(&context->thread, context->thread_stack,
			CONFIG_ETH_W5500_RX_THREAD_STACK_SIZE,
			(k_thread_entry_t)w5500_rx_thread,
			(void *)dev, NULL, NULL,
			K_PRIO_COOP(CONFIG_ETH_W5500_RX_THREAD_PRIO),
			0, K_NO_WAIT);

	context->suspended = false;
	LOG_INF("W5500 Initialized");
	return 0;
}

static struct w5500_runtime w5500_0_runtime = {
	.tx_rx_sem = Z_SEM_INITIALIZER(w5500_0_runtime.tx_rx_sem,
				       1,  UINT_MAX),
	.int_sem  = Z_SEM_INITIALIZER(w5500_0_runtime.int_sem,
				      0, UINT_MAX),
};

static const struct w5500_config w5500_0_config = {
	.gpio_port = DT_INST_GPIO_LABEL(0, int_gpios),
	.gpio_pin = DT_INST_GPIO_PIN(0, int_gpios),
	.gpio_flags = DT_INST_GPIO_FLAGS(0, int_gpios),
	.spi_port = DT_INST_BUS_LABEL(0),
	.spi_freq  = DT_INST_PROP(0, spi_max_frequency),
	.spi_slave = DT_INST_REG_ADDR(0),
#if DT_INST_SPI_DEV_HAS_CS_GPIOS(0)
	.spi_cs_port = DT_INST_SPI_DEV_CS_GPIOS_LABEL(0),
	.spi_cs_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(0),
#endif
	.timeout = CONFIG_ETH_W5500_TIMEOUT,
};

ETH_NET_DEVICE_INIT(w5500_0, DT_INST_LABEL(0),
		    w5500_init, device_pm_control_nop,
		    &w5500_0_runtime, &w5500_0_config,
		    CONFIG_ETH_INIT_PRIORITY, &api_funcs, NET_ETH_MTU);


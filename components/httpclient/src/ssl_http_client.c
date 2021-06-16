/******************************************************************************************************************************
*              MODULE IMPLEMENTATION FILE
******************************************************************************************************************************
*  COPYRIGHT (C) 2019 ASR Corporation.
*
*  This file and the software in it is furnished under
*  license and may only be used or copied in accordance with the terms of the
*  license. The information in this file is furnished for informational use
*  only, is subject to change without notice, and should not be construed as
*  a commitment by ASR Corporation. ASR Corporation assumes no
*  responsibility or liability for any errors or inaccuracies that may appear
*  in this document or any software that may be provided in association with
*  this document.
*  Except as permitted by such license, no part of this document may be
*  reproduced, stored in a retrieval system, or transmitted in any form or by
*  any means without the express written consent of ASR Corporation.
*
*  Title:   HTTPS Package HTTPS API
*
*  Filename:    ssl_http_client.c
*  Target, subsystem: Common Platform, HAL
*
*  Authors:	Roy Shi
*
*  Description: This file includes all the API that the https Package support
*
*  Last Modified: <Initial> <date>
*
*  Notes:
*********************************************************************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "ssl_http_client.h"
#include "httpclient_sys.h"
#include "rt_port.h"
//#include "mbedtls/config-no-entropy.h"

#define ssl_debug(fmt, args...) do { printf("[ssl]"fmt, ##args); } while(0)

#ifdef MBEDTLS_X509_USE_C
//#define CA_CERT
//#define OWN_CERT
#endif

#ifdef CA_CERT
const unsigned char ca1_cert[] = {
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n"
};
#endif

#ifdef OWN_CERT
const unsigned char client_cert[] = {
};

const unsigned char client_key[] = {
};

#endif
static unsigned int havege_rand(void *unused, unsigned char * unusedstr, size_t unusedt)
{
	char * p = (char *)malloc(16);
	unsigned int rand = (unsigned int)p;
	free(p);
	if ((rand & 0xff) == 0) {
		rand |= 0x96;
	}
	return rand;
}

static void debug(void *userdata, int level,
                     const char *filename, int line,
                     const char *msg)
{
	//ssl_debug("%s", msg);
}

static int ssl_client_recv(void * ctx, unsigned char * buf, unsigned int len)
{
	struct ssl_client * client = (struct ssl_client *)ctx;
	return recv(client->fd, buf, len, 0);
}

static int ssl_client_send(void * ctx, const unsigned char *buf, unsigned int len)
{
    struct ssl_client * client = (struct ssl_client *)ctx;
	return send(client->fd, buf, len, 0);
}

#ifdef CA_CERT
#define HOSTNAME "asr"
#endif
struct ssl_client * ssl_client_init(int fd)
{
	char *pers = "ssl_client";
	int ret;
	struct ssl_client * client = (struct ssl_client *)malloc(sizeof(struct ssl_client));
	if (!client) {
		http_error("Cannot malloc memory(ssl_client_init)\n");
		return NULL;
	}
	
	memset(client, 0, sizeof(struct ssl_client));

#ifdef CA_CERT
	mbedtls_x509_crt *ca_chain; 
	ca_chain = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
	memset(ca_chain, 0, sizeof(mbedtls_x509_crt));
#endif

	mbedtls_entropy_init(&client->entropy);
	mbedtls_ctr_drbg_init(&client->ctr_drbg);
	if((ret = mbedtls_ctr_drbg_seed(&client->ctr_drbg, mbedtls_entropy_func, 
                                    &client->entropy,
                                    (unsigned char *)pers, strlen(pers))) != 0)
	{
		http_error( " failed\n  ! ctr_drbg_init returned %d\n", ret );
		goto failed;
	}

	mbedtls_ssl_init(&(client->ssl));
	mbedtls_ssl_config_init(&(client->config));
	client->fd = fd;
	mbedtls_ssl_conf_rng(&(client->config), mbedtls_ctr_drbg_random, &client->ctr_drbg);

#ifdef CA_CERT
    mbedtls_x509_crt_parse(ca_chain,ca1_cert,strlen(ca1_cert));
	mbedtls_ssl_conf_ca_chain(&(client->config), ca_chain, NULL);
    mbedtls_ssl_set_hostname(&(client->ssl), HOSTNAME);
#endif

#ifdef OWN_CERT
    mbedtls_x509_crt *client_chain = NULL;
    mbedtls_pk_context *client_rsa = NULL;

    client_chain = (mbedtls_x509_crt *)malloc(sizeof(mbedtls_x509_crt));
    memset(client_chain, 0, sizeof(mbedtls_x509_crt));
    mbedtls_x509_crt_parse(client_chain,client_cert,strlen(client_cert));
    
    client_rsa = (mbedtls_pk_context *)malloc(sizeof(mbedtls_pk_context));
    memset(client_rsa, 0, sizeof(mbedtls_rsa_context));  
    mbedtls_pk_parse_key(client_rsa, client_key, strlen(client_key), NULL,0);
    mbedtls_ssl_conf_own_cert(&client->config, client_chain, client_rsa);
#endif

	//mbedtls_ssl_conf_endpoint(&(client->config), MBEDTLS_SSL_IS_CLIENT);
	mbedtls_ssl_config_defaults(&client->config, MBEDTLS_SSL_IS_CLIENT,
								MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);

#ifdef CA_CERT
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_REQUIRED);
#else
	mbedtls_ssl_conf_authmode(&client->config, MBEDTLS_SSL_VERIFY_OPTIONAL);
#endif

	mbedtls_ssl_conf_rng(&client->config, mbedtls_ctr_drbg_random, &client->ctr_drbg);
	mbedtls_ssl_conf_dbg(&client->config, debug, NULL);
	//mbedtls_debug_set_threshold(1000);
	mbedtls_ssl_set_bio(&client->ssl, client,
                        ssl_client_send, ssl_client_recv, NULL);

	mbedtls_ssl_setup(&client->ssl, &client->config);
	return client;
failed:
#ifdef CA_CERT
	if(ca_chain) free(ca_chain);
#endif
	if (client) free(client);
	return NULL;
}




void ssl_client_shutdown(struct ssl_client * client)
{

    mbedtls_ssl_config *config=NULL;

	if (!client) return;
#ifdef CA_CERT
    config = (mbedtls_ssl_config *)&(client->config);
	if(config->ca_chain){
        free(config->ca_chain);
	}
#endif

#ifdef OWN_CERT
    mbedtls_ssl_key_cert * cert=NULL;
    config = (mbedtls_ssl_config *)&(client->config);
    cert = config->key_cert;
    
	if(cert&&cert->cert){
        free(cert->cert);
        cert->cert=NULL;
	}
    if(cert&&cert->key){
        free(cert->key);
        cert->key=NULL;
	}
#endif

	http_debug("ssl close notify");
	mbedtls_ssl_close_notify(&client->ssl);
	http_debug("ssl ctr drbg free");
	mbedtls_ctr_drbg_free(&client->ctr_drbg);
	http_debug("ssl entropy free");
	mbedtls_entropy_free(&client->entropy);
	http_debug("ssl free");
	mbedtls_ssl_free(&client->ssl);
	http_debug("ssl config free");
	mbedtls_ssl_config_free(&client->config);
	http_debug("client free");
	free(client);
	client = NULL;
	http_debug("shutdown end");
}

int ssl_client_write( void *ssl, const unsigned char *buf, int len )
{
    return ssl_write(ssl, buf, len );
}

int ssl_client_read( void *ssl, unsigned char *buf, int len )
{
    return ssl_read( ssl, buf, len );
}


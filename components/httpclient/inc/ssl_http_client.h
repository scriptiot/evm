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
*  Filename:    ssl_http_client.h
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


#ifndef __SSL_HTTP_CLIENT_H__
#define __SSL_HTTP_CLIENT_H__
#include "ssl.h"
#include "ssl_internal.h"
#include "compat-1.3.h" // for polar -> mbedtls
#include "entropy.h"
#include "ctr_drbg.h"

struct ssl_client {
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config config;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_entropy_context entropy;
	int fd;
};

extern struct ssl_client * ssl_client_init(int fd);

extern void ssl_client_shutdown(struct ssl_client * client);

extern int ssl_client_write( void *ssl, const unsigned char *buf, int len );

extern int ssl_client_read( void *ssl, unsigned char *buf, int len );

#endif

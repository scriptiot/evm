#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "digest.h"
#include "http_client_base64.h"
#include "md5.h"
#include "compat-1.3.h" // for polar -> mbedtls
#include "httpclient_sys.h"

void md5it(unsigned char *outbuffer, /* 16 bytes */
		   const unsigned char *input)
{
	md5_context * ctx = (md5_context *)HTTP_CLIENT_MALLOC(sizeof(md5_context));
	int len = strlen((const char *)input);
	if (!ctx) {
		http_error("Cannot malloc memory for md5it!");
		return;
	}
	memset(ctx, 0, sizeof(md5_context));
	md5_starts(ctx);
	md5_update(ctx, input, len);
	md5_finish(ctx, outbuffer);
	HTTP_CLIENT_FREE(ctx);
}

static void md5_to_ascii(unsigned char *source, unsigned char *dest)
{
	int i;
	memset(dest, 0, 33);
	for(i=0; i<16; i++)
		sprintf((char *)(dest + (i*2)), "%02x", source[i]);
}

int clear_digestdata(struct digestdata * data)
{
	// Free all memory
	if (data->algorithm) HTTP_CLIENT_FREE(data->algorithm);
	if (data->qop) HTTP_CLIENT_FREE(data->qop);
	if (data->opaque) HTTP_CLIENT_FREE(data->opaque);
	if (data->realm) HTTP_CLIENT_FREE(data->realm);
	if (data->cnonce) HTTP_CLIENT_FREE(data->cnonce);
	if (data->nonce) HTTP_CLIENT_FREE(data->nonce);
	if (data->username) HTTP_CLIENT_FREE(data->username);
	if (data->uri) HTTP_CLIENT_FREE(data->uri);
	if (data->response) HTTP_CLIENT_FREE(data->response);
	HTTP_CLIENT_FREE(data);
	return 0;
}

// Return value:
//   0 -> Break parse; 1 -> Continue parse next content
static int parse_content(const char ** content, char ** ppname, char ** ppvalue)
{
	const char * p = *content;
	char * name = *ppname, * value = *ppvalue;
	int i = 0, state = 0;

	while (*p == ' ' || *p == '\n' || *p == '\r' || *p == ',')
		p++;

	while (*p != '\0') {
		switch (state) {
		case 0: /* Name state */
			if (*p == '=') {
				state = 1;
				name[i] = '\0';
				i = 0;
				break;
			}
			name[i++] = *p;
			break;
		default:/* Value state */
			if (*p == '"') {
				if (state == 1) {
					// Handle first '"'
					state++;
					break;
				} else if (state == 2) {
					// Seconde '"' means string end
					value[i] = '\0';
					p++;
					goto done;
				}
				break;
			} else {
				if (*p == ',' && state == 1) {
					// Maybe not string mode, like cnt=000001,
					value[i] = '\0';
					p++;
					goto done;
				}
				value[i++] = *p;
			}
			break;
		}
		p++;
	}
done:
	*content = p;
	return i ? 1 : 0;
}

int digest_input(const char * header, struct digestdata ** data)
{
	struct digestdata * d;
	char * name = (char *)HTTP_CLIENT_MALLOC(128);
	char * value = (char *)HTTP_CLIENT_MALLOC(128);
	int err = 0;

	if (!name || !value) {
		http_error("Cannot malloc memory for digest input!\n");
		goto done;
	}

	if (*data) {
		clear_digestdata(*data);
	}

	d = *data = (struct digestdata *)HTTP_CLIENT_MALLOC(sizeof(struct digestdata));
	if (!d) {
		http_error("Malloc failed! @%d\n", __LINE__);
		err = -1;
		goto done;
	}
	memset(d, 0, sizeof(struct digestdata));

	while (parse_content(&header, &name, &value)) {
		http_error("Get digest with %s=%s\n\n", name, value);
		if (!strcmp(name, "nonce")) {
		    if(d->nonce) HTTP_CLIENT_FREE(d->nonce);
			d->nonce = strdup(value);
		} else if (!strcmp(name, "realm")) {
		    if(d->realm) HTTP_CLIENT_FREE(d->realm);
			d->realm = strdup(value);
		} else if (!strcmp(name, "qop")) {
		    if(d->qop) HTTP_CLIENT_FREE(d->qop);
			d->qop = strdup(value);
		} else if (!strcmp(name, "opaque")) {
		    if(d->opaque) HTTP_CLIENT_FREE(d->opaque);
			d->opaque = strdup(value);
		} else if (!strcmp(name, "username")) {
		    if(d->username) HTTP_CLIENT_FREE(d->username);
			d->username = strdup(value);
		} else if (!strcmp(name, "uri")) {
		    if(d->uri) HTTP_CLIENT_FREE(d->uri);
			d->uri = strdup(value);
		} else if (!strcmp(name, "response")) {
		    if(d->response) HTTP_CLIENT_FREE(d->response);
			d->response = strdup(value);
		} else if (!strcmp(name, "nc")) {
			if (1 != sscanf(value, "%d", &d->nc)) {
				d->nc = 0;
			}
		} else if (!strcmp(name, "cnonce")) {
		    if(d->cnonce) HTTP_CLIENT_FREE(d->cnonce);
			d->cnonce = strdup(value);
		}	else {
			http_error("Handle unkonwn content in digest input(%s:%s)...\n", name, value);
		}
	}

done:
	if (value) HTTP_CLIENT_FREE(value);
	if (name) HTTP_CLIENT_FREE(name);
	if (err != 0 && data) {
		HTTP_CLIENT_FREE(*data);
		*data = NULL;
	}
	return err;
}

int digest_output(const char * name, const char * password, const char * request, 
	const char *uri, struct digestdata * d, char **outptr, int *outlen)
{
	#define TEMP_BUF_SIZE 256
	char * tempbuf = NULL, * p = NULL;
	unsigned char * md5buf = (unsigned char *)HTTP_CLIENT_MALLOC(16);
	unsigned char * ha1 = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	unsigned char * ha2 = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	unsigned char * request_digest = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	static unsigned int test = 33262;

	if (!md5buf || !ha1 || !ha2 || !request_digest) {
		http_error("Cannot malloc memory for digest output!\n");
		*outptr = NULL;
		*outlen = 0;
		goto done;
	}

	tempbuf = (char *)HTTP_CLIENT_MALLOC(TEMP_BUF_SIZE);
	if (!tempbuf) {
		http_error("Malloc failed! @%d\n", __LINE__);
		goto done;
	}
	memset(tempbuf, 0, TEMP_BUF_SIZE);

	if (!d->nc)
		d->nc = 1;
	/* Make "name:relam:password" to A1 */
	sprintf(tempbuf, "%s:%s:%s", name, d->realm, password);
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, ha1);

	/* Make "request:uri" to A2 */
	memset(tempbuf, 0, TEMP_BUF_SIZE);
	sprintf(tempbuf, "%s:%s", request, uri);
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, ha2);

	if (!d->cnonce) {
		/* If server not set cnonce, make client cnonce */
		int cnonce_len;
	    sprintf(tempbuf, "%06ld", test);
	    http_client_base64_encode(tempbuf, &d->cnonce, &cnonce_len,0);
	}

	/* Accroding A1 and A2 make request digest */
	if (d->qop) {
		sprintf(tempbuf, "%s:%s:%08x:%s:%s:%s", 
			ha1, d->nonce, d->nc, d->cnonce, d->qop, ha2);
	} else {
		sprintf(tempbuf, "%s:%s:%s", ha1, d->nonce, ha2);
	}
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, request_digest);
	HTTP_CLIENT_FREE(tempbuf);
	tempbuf = NULL;


	/*roy add for memory leak issue 20160322*/
	if (*outptr){
		HTTP_CLIENT_FREE(*outptr);
		http_error("Clear old auth response!");
	}
	/* Malloc output buffer, and format result to buffer */
	p = *outptr = (char *)HTTP_CLIENT_MALLOC(512);
	memset(p, 0, 512);
	*outlen = 0;
	if (d->qop) {
		*outlen = sprintf(p, "Authorization: Digest "
							 "username=\"%s\", "
							 "realm=\"%s\", "
							 "algorithm=\"MD5\", "
							 "nonce=\"%s\", "
							 "uri=\"%s\", "
							 "cnonce=\"%s\", "
							 "nc=%08x, "
							 "qop=\"%s\"",
							 name, d->realm, d->nonce,
							 uri, d->cnonce, d->nc,
							 d->qop);
	} else {
		*outlen = sprintf(p, "Authorization: Digest "
							 "username=\"%s\","
							 "realm=\"%s\","
							 "nonce=\"%s\","
							 "uri=\"%s\"",
							 name, d->realm, d->nonce, uri);
	}
	if (d->opaque) {
		*outlen += sprintf(p + (*outlen), ", opaque=\"%s\"", d->opaque);
	}

	*outlen += sprintf(p + (*outlen), ", response=\"%s\"", request_digest);
	*outlen += sprintf(p + (*outlen), "\r\n");
	//printf("****%s***\n", p);
done:
	if (tempbuf) HTTP_CLIENT_FREE(tempbuf);
	if (md5buf) HTTP_CLIENT_FREE(md5buf);
	if (ha1) HTTP_CLIENT_FREE(ha1);
	if (ha2) HTTP_CLIENT_FREE(ha2);
	if (request_digest) HTTP_CLIENT_FREE(request_digest);
	return 0;
}

int digest_authenticate(const char * name, const char * password, const char * method,
	struct digestdata * d)
{
	#undef TEMP_BUF_SIZE
	#define TEMP_BUF_SIZE 256
	char * tempbuf = NULL, * p = NULL;
	unsigned char * md5buf = (unsigned char *)HTTP_CLIENT_MALLOC(16);
	memset(md5buf, 0, 16);
	unsigned char * ha1 = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	memset(ha1, 0, 33);
	unsigned char * ha2 = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	memset(ha2, 0, 33);
	unsigned char * request_digest = (unsigned char *)HTTP_CLIENT_MALLOC(33);
	memset(request_digest, 0, 33);
	
	static unsigned int test = 33262;
	int ret=0;
	
	if (!md5buf || !ha1 || !ha2 || !request_digest) {
		http_error("Cannot malloc memory for digest output!\n");
		goto done;
	}
	
	tempbuf = (char *)HTTP_CLIENT_MALLOC(TEMP_BUF_SIZE);
	if (!tempbuf) {
		http_error("Malloc failed! @%d\n", __LINE__);
		goto done;
	}
	memset(tempbuf, 0, TEMP_BUF_SIZE);

	http_debug("digest input: name[%s], psw[%s], method[%s]", name, password, method);

	if (!d->nc)
		d->nc = 1;
	/* Make "name:relam:password" to A1 */
	sprintf(tempbuf, "%s:%s:%s", name, d->realm, password);
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, ha1);

	/* Make "request:uri" to A2 */
	memset(tempbuf, 0, TEMP_BUF_SIZE);
	sprintf(tempbuf, "%s:%s", method, d->uri);
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, ha2);

	if (!d->cnonce) {
		/* If server not set cnonce, make client cnonce */
		int cnonce_len;
		sprintf(tempbuf, "%06ld", test);
		http_client_base64_encode(tempbuf, &d->cnonce, &cnonce_len,0);
	}

	/* Accroding A1 and A2 make request digest */
	if (d->qop) {
		sprintf(tempbuf, "%s:%s:%08x:%s:%s:%s", 
			ha1, d->nonce, d->nc, d->cnonce, d->qop, ha2);
	} else {
		sprintf(tempbuf, "%s:%s:%s", ha1, d->nonce, ha2);
	}
	md5it(md5buf, (const unsigned char *)tempbuf);
	md5_to_ascii(md5buf, request_digest);
	HTTP_CLIENT_FREE(tempbuf);
	tempbuf = NULL;

	ret = memcmp(request_digest, d->response, 33);
	http_debug("digest_authenticate %s:%s", request_digest, d->response);
done:
	if (tempbuf) HTTP_CLIENT_FREE(tempbuf);
	if (md5buf) HTTP_CLIENT_FREE(md5buf);
	if (ha1) HTTP_CLIENT_FREE(ha1);
	if (ha2) HTTP_CLIENT_FREE(ha2);
	if (request_digest) HTTP_CLIENT_FREE(request_digest);
	return ret;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http_client_base64.h"
#include "libhttpclient.h"
#include "httpclient_sys.h"



static const char table64[]=
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int http_client_base64_encode(const char * src, char **outptr, int *outlen,int prepass)
{
	unsigned char ibuf[3];
	unsigned char obuf[4];
	int i;
	int inputparts;
	char *output;
	char *base64data;
	char *convbuf = NULL;
	int insize;

	const char *indata = src;

	*outptr = NULL;
	*outlen = 0;

	if(prepass){
		insize = 16;//fixed bug
	}else{
		insize = strlen(src);
	}
	
	http_debug("%s @%d : prepass=%d\n",__func__, insize,prepass);
	

	
	base64data = output = (char *)HTTP_CLIENT_MALLOC(insize * 4 + 4);
	if (NULL == output) {
		http_error("Can not malloc memory @%d\n", __LINE__);
		return HTTP_CLIENT_ERROR;
	}

	while (insize > 0) {
		for (i = inputparts = 0; i < 3; i++) {
			if (insize > 0) {
				inputparts++;
				ibuf[i] = (unsigned char) * indata;
				indata++;
				insize--;
			} else {
				ibuf[i] = 0;
			}
		}
		obuf[0] = (unsigned char)((ibuf[0] & 0xFC) >> 2);
		obuf[1] = (unsigned char)(((ibuf[0] & 0x03) << 4) | \
								((ibuf[1] & 0xF0) >> 4));
		obuf[2] = (unsigned char)(((ibuf[1] & 0x0F) << 2) | \
								((ibuf[2] & 0xC0) >> 6));
		obuf[3] = (unsigned char)(ibuf[2] & 0x3F);
		switch(inputparts) {
			case 1:
				sprintf(output, "%c%c==", table64[obuf[0]], table64[obuf[1]]);
				break;
			case 2:
				sprintf(output, "%c%c%c=", table64[obuf[0]], table64[obuf[1]],
											table64[obuf[2]]);
				break;
			default:
				sprintf(output, "%c%c%c%c", table64[obuf[0]], table64[obuf[1]], 
											table64[obuf[2]], table64[obuf[3]]);
				break;
		}
		output += 4;
	}
	*output = '\0';
	*outptr = base64data;
	*outlen = strlen(base64data);
	return HTTP_CLIENT_OK;
}

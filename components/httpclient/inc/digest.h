#ifndef __DIGEST_H__
#define __DIGEST_H__

struct digestdata {
	char *nonce;
	char *cnonce;
	char *realm;
	int algo;
	int stale; /* set true for re-negotiation */
	char *opaque;
	char *qop;
	char *algorithm;
	int nc; /* nounce count */
	char *username;
	char *uri;
	char *response;
};

extern int digest_input(const char * header, struct digestdata ** data);

extern int digest_output(const char * name, const char * password, const char * request, 
	const char *uri, struct digestdata * d, char **outptr, int *outlen);

extern int clear_digestdata(struct digestdata * data);

extern  int digest_authenticate(const char * name, const char * password, const char * method,
	struct digestdata * d);

#endif

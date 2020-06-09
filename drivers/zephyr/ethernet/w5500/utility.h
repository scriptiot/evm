#ifndef _UTILITY_H
#define _UTILITY_H
#include "stdio.h"
#include "types.h"


void w5500_delay_s( uint32 time_s );
void w5500_delay_us(uint32 time_us);
void w5500_delay_ms(uint32 time_ms);

uint16 atoi16(char* str,uint16 base); 			      				/* Convert a string to integer number */
uint32 atoi32(char* str,uint16 base); 			     	 				/* Convert a string to integer number */
void itoa(uint16 n,uint8* str, uint8 len);
int validatoi(char* str, int base, int* ret); 						/* Verify character string and Convert it to (hexa-)decimal. */
char c2d(u_char c); 					                    				/* Convert a character to HEX */

uint16 swaps(uint16 i);
uint32 swapl(uint32 l);

void replacetochar(char * str, char oldchar, char newchar);

void mid(int8* src, int8* s1, int8* s2, int8* sub);
void inet_addr_(unsigned char* addr,unsigned char *ip);

char* inet_ntoa(unsigned long addr);			                /* Convert 32bit Address into Dotted Decimal Format */
char* inet_ntoa_pad(unsigned long addr);

uint32 inet_addr(unsigned char* addr);		                /* Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address */

char verify_ip_address(char* src, uint8 * ip);            /* Verify dotted notation IP address string */

uint16 htons( unsigned short hostshort);	                /* htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).*/

uint32 htonl(unsigned long hostlong);	          	        /* htonl function converts a unsigned long from host to TCP/IP network byte order (which is big-endian). */
 
uint32 ntohs(unsigned short netshort);		                /* ntohs function converts a unsigned short from TCP/IP network byte order to host byte order (which is little-endian on Intel processors). */

uint32 ntohl(unsigned long netlong);		                  /* ntohl function converts a u_long from TCP/IP network order to host byte order (which is little-endian on Intel processors). */

uint16 checksum(unsigned char * src, unsigned int len);		/* Calculate checksum of a stream */

uint8 check_dest_in_local(u_long destip);			            /* Check Destination in local or remote */

#endif

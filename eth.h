#ifndef __ETH_H__
#define __ETH_H__

void init_eth(char *iface_);
void handle_eth();

void printmac(FILE *fout, u_int8_t *b);
void printipv4(FILE *fout, char *b);
void printipv6(char *b);

extern char iface[100];
extern char macaddr[6];//local

#endif

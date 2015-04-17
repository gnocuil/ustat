#ifndef __ETH_H__
#define __ETH_H__

void init_eth(char *iface_);
void handle_eth();

void printmac(u_int8_t *b);

extern char iface[100];

#endif

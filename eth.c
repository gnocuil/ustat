#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <net/ethernet.h> /* the L2 protocols */
#include "eth.h"
#include "data.h"

static int fd;
char iface[100];

char macaddr[6];//local

static void getmacaddr()
{
    struct ifreq buffer;
    int s = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&buffer, 0, sizeof(buffer));
    strcpy(buffer.ifr_name, iface);
    ioctl(s, SIOCGIFHWADDR, &buffer);
    close(s);
    memcpy(macaddr, buffer.ifr_hwaddr.sa_data, 6);
}

void printmac(FILE *fout, u_int8_t *b) {
    int i;
    for (i = 0; i < 6; ++i) {
        if (i) fprintf(fout, ":");
        fprintf(fout, "%02x", b[i]&0xFF);
    }
}

void printipv4(FILE *fout, char *b) {
    int i;
    for (i = 0; i < 4; ++i) {
        if (i) fprintf(fout, ".");
        fprintf(fout, "%d", (uint8_t)b[i]);
    }
}

void printipv6(char *b) {
    int i;
    for (i = 0; i < 16; ++i) {
        if (i && i % 2 == 0) printf(":");
        printf("%02x", (uint8_t)b[i]);
    }
}

void init_eth(char *iface_)
{
    strcpy(iface, iface_);
    struct sockaddr_ll device;
    if ((device.sll_ifindex = if_nametoindex(iface)) <= 0) {
        fprintf(stderr, "Failed to resolve the index of %s\n", iface);
        exit(-1);
    }
    device.sll_family = AF_PACKET;
    device.sll_protocol = htons(ETH_P_ALL);
    fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (bind(fd, (struct sockaddr*)&device, sizeof(device)) < 0) {
        fprintf(stderr, "Failed to bind...\n");
        exit(1);
    }
    getmacaddr();
}
#define OUT 0
void handle_eth()
{
    char buf[4000];
    int count = read(fd, buf, 60);
    if (OUT) printf("read %d bytes\n", count);
    struct ether_header *eth = (struct ether_header*)buf;
    
    int len = count;
    if (ntohs(eth->ether_type) == ETHERTYPE_IP) {
        len = ntohs(*(uint16_t*)(buf + 14 + 2)) + 14;
        printf("v4 len=%d\n", len);
        add(eth->ether_shost, buf + 14 + 12, 4, len, UP);
        add(eth->ether_dhost, buf + 14 + 16, 4, len, DOWN);
    } else if (ntohs(eth->ether_type) == ETHERTYPE_IPV6) {
        len = ntohs(*(uint16_t*)(buf + 14 + 4)) + 14 + 40;
        printf("v6 len=%d\n", len);
        add(eth->ether_shost, buf + 14 + 8, 16, len, UP);
        add(eth->ether_dhost, buf + 14 + 24, 16, len, DOWN);
    }
}


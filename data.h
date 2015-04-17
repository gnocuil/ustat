#ifndef __DATA_H__
#define __DATA_H__

#include <stdint.h>

#define UP 0
#define DOWN 1

struct Record {
    int addrlen;
    char addr[16];
    uint64_t bytes_up, bytes_down;
    struct Record *next;
};

struct Maclist {
    char addr[6];
    struct Record *records;
    struct Maclist *next;
};
extern struct Maclist* maclists[256];


void add(char *mac, char *addr, int addrlen, int datalen, int dir);
void printJson();

char path[100];//path to the output json file
char path_tmp[100];//path to the output json file
extern int allow_local;


#endif

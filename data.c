#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "data.h"
#include "eth.h"

struct Maclist* maclists[256];
int allow_local = 0;

static pthread_rwlock_t  rwlock = PTHREAD_RWLOCK_INITIALIZER;

static void dump(struct Maclist* mac, struct Record* record)
{
    printmac(stdout, mac->addr);
    printf("  ");
    if (record->addrlen == 4)
        printipv4(stdout, record->addr);
    else
        printipv6(record->addr);
    printf("   ");
    printf("%lld %lld\n", record->bytes_up, record->bytes_down);
}

static struct Maclist* find_maclist(char *mac)
{
    uint8_t index = 0;
    int j;
    for (j = 0; j < 6; ++j) index += (uint8_t)mac[j];
    struct Maclist *maclist = maclists[index];
    while (maclist) {
        if (memcmp(maclist->addr, mac, 6) == 0)
            return maclist;
        maclist = maclist->next;
    }
    struct Maclist *newmac = (struct Maclist*)malloc(sizeof(struct Maclist));
    memcpy(newmac->addr, mac, 6);
    newmac->records = NULL;
    newmac->next = maclists[index];
    pthread_rwlock_wrlock(&rwlock);
    maclists[index] = newmac;
    pthread_rwlock_unlock(&rwlock);
    return newmac;
}


void add(char *mac, char *addr, int addrlen, int datalen, int dir)
{
    if (!allow_local && memcmp(macaddr, mac, 6) == 0)//skip local
        return;
    struct Maclist* maclist = find_maclist(mac);
    struct Record* record = maclist->records;
    while (record) {
        if (memcmp(record->addr, addr, addrlen) != 0) {
            record = record->next;
            continue;
        }
        break;
    }
    if (!record) {
        record = (struct Record*)malloc(sizeof(struct Record));
        record->addrlen = addrlen;
        memcpy(record->addr, addr, addrlen);
        record->bytes_up = 0;
        record->bytes_down = 0;
        record->next = maclist->records;
        pthread_rwlock_wrlock(&rwlock);
        maclist->records = record;
        pthread_rwlock_unlock(&rwlock);
    }
    if (dir == UP)
        record->bytes_up += datalen;
    else
        record->bytes_down += datalen;
    //dump(maclist, record);
}

void printJson()
{
    pthread_rwlock_rdlock(&rwlock);
    if (strlen(path_tmp) < 5) return;
    FILE *fout = fopen(path_tmp, "w");
    if (!fout) return;
    fprintf(fout, "{\n");
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long current_time = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    fprintf(fout, "\"time\": %lld,\n", current_time);
    fprintf(fout, "\"maclist\": [");
    int i;
    int hasmac = 0;
    for (i = 0; i < 256; ++i) {
        struct Maclist *maclist = maclists[i];
        while (maclist) {
            if (hasmac) fprintf(fout, ",");
            fprintf(fout, "\n  {\n");
            fprintf(fout, "    \"macaddr\": \"");
            printmac(fout, maclist->addr);
            fprintf(fout, "\",\n");
            fprintf(fout, "    \"ip\": [");
            
            struct Record* record = maclist->records;
            int hasrecord = 0;
            while (record) {
                if (hasrecord) fprintf(fout, ",");
                hasrecord = 1;
                fprintf(fout, "\n      {\n");
                fprintf(fout, "        \"ipaddr\": \"");
                if (record->addrlen == 4)
                    printipv4(fout, record->addr);
                else {
                    char str[INET6_ADDRSTRLEN];
                    inet_ntop(AF_INET6, record->addr, str, INET6_ADDRSTRLEN);
                    fprintf(fout, "%s", str);
                }
                fprintf(fout, "\",\n");
                fprintf(fout, "        \"upstream-bytes\": %lld,\n", record->bytes_up);
                fprintf(fout, "        \"downstream-bytes\": %lld\n", record->bytes_down);
                fprintf(fout, "      }");
                record = record->next;
            }
            
            fprintf(fout, "]\n  }");
            hasmac = 1;
            maclist = maclist->next;
        }
    }
    
    fprintf(fout, "\n]}\n");
    fclose(fout);
    char cmd[1000] = {0};
    sprintf(cmd, "mv %s %s", path_tmp, path);
    system(cmd);
    
    pthread_rwlock_unlock(&rwlock);
}


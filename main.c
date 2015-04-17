#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "eth.h"
#include "data.h"

static void usage()
{
	fprintf(stderr, "Usage: ustat [options] <LISTEN_INTERFACE_NAME>\n");
	fprintf(stderr, "  options: -o <FILE_PATH>       output json file path\n");
	fprintf(stderr, "           -a                   allow recording local macaddr\n");
	exit(1);
}

static void* json_thread()
{
    while (1) {
        usleep(500000);
        printJson();
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
        usage();
    int i;
    for (i = 1; i + 2 < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            strcpy(path, argv[++i]);
            printf("output file: %s\n", path);
            strcpy(path_tmp, path);
            strcat(path_tmp, ".tmp");
            printf("temp output file: %s\n", path_tmp);
        } else if (strcmp(argv[i], "-a") == 0) {
            puts("Allow recording packets to/from local mac-addr!");
            allow_local = 1;
        }
    }
    init_eth(argv[argc - 1]);
    printf("Listen on interface: %s\n", iface);
    
    pthread_t tid;
    pthread_create(&tid, NULL, json_thread, NULL);
    
    while (1) {
        handle_eth();
    }
}

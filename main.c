#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eth.h"

char path[100];

static void usage()
{
	fprintf(stderr, "Usage: ustat [options] <LISTEN_INTERFACE_NAME>\n");
	fprintf(stderr, "  options: -o <FILE_PATH>       output json file path\n");
//	fprintf(stderr, "           --mtu <MTU_VALUE>          default: %d\n", DEFAULT_MTU);
	exit(1);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        usage();
    for (int i = 1; i + 2 < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            strcpy(path, argv[++i]);
            printf("output file: %s\n", path);
        }
    }
    init_eth(argv[argc - 1]);
    printf("Listen on interface: %s\n", iface);
    while (true) {
        handle_eth();
    }
}

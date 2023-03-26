#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "nrftw.h"
#include "plugin_api.h"
#include "plugin_handler.h"



int parse_options(int argc, char **argv, const char *opts, const struct option *longopts) {
    int longindex;
    while (1) {
        switch (getopt_long(argc, argv, opts, longopts, &longindex)) {
            case 0:
                * (char**) longopts[longindex].flag = optarg;
                break;
            case 'P':
                printf("path = %s\n", optarg);  //?
                break;
            case 'A':
                printf("and\n");    //?
                break;
            case 'O':
                printf("or\n");     //?
                break;
            case 'N':
                printf("not\n");    //?
                break;
            case 'v':
                fprintf(stdout, "version\n");
                return 1;
            case 'h':
                fprintf(stdout, "help\n");
                return 1;
            case -1:
                return 0;
            default:
                return 1;

        }
    }
}


int main(int argc, char **argv) {

    struct required_plugins rp = rpload(argc, argv, ".");

    if (!parse_options(argc, argv, "P:AONvh", rp.opts)) {
        for (int i = 0; i < rp.dls_len; ++i) {
            int (*fun)(const char*, struct option[], size_t) = dlsym(rp.dls[i], "plugin_process_file");
            (*fun)("", rp.opts, 1);
        }
    }

    rpclose(rp);
    return 0;
}

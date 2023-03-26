#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "nrftw.h"
#include "plugin_api.h"
#include "plugin_handler.h"



int mode = 0b00;
struct required_plugins rp;

int run_rp(const char *path) {
    for (int i = 0; i < rp.dls_len; ++i)
        if ((*rp.ppfs[i])(path, rp.opts, rp.opts_len) == (mode&1))
            return !!(mode&2);
    return !(mode&2);
}

void walk_func(const char *path) {
    if (run_rp(path)) printf("+");
    else printf("-");
    printf(" %s\n", path);
}


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
                mode = 0b00;
                break;
            case 'O':
                mode = 0b11;
                break;
            case 'N':
                mode ^= 0b1;
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

    rp = rpload(argc, argv, ".");

    if (parse_options(argc, argv, "P:AONvh", rp.opts)) {
        rpclose(rp);
        return 1;
    }

    nrftw("example", &walk_func);

    rpclose(rp);
    return 0;
}

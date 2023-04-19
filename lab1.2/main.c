#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "nrftw.h"
#include "plugin_api.h"
#include "plugin_handler.h"



int magic = 0;
struct required_plugins rp;

int run_rp(const char *path) {
    for (int i = 0; i < rp.dls_len; ++i) {
        int r = (*rp.ppfs[i])(path, rp.opts, rp.opts_len);
        if (r == (magic&1)) return !!(magic&2);
        if (r < 0) return -1;
    }
    return !(magic&2);
}

void walk_func(const char *path) {
    switch (run_rp(path)) {
        case 0:
            printf("-");
            break;
        case 1:
            printf("+");
            break;
        case -1:
            printf("?");
            break;
    }
    printf(" %s\n", path);
}


char* get_plugins_path(int argc, char **argv) {
    for (int i = 0; i < argc-1; ++i)
        if (argv[i][0] == '-' && argv[i][1] == 'P' && argv[i][2] == '\0')
            return argv[i+1];
    return ".";
}


int parse_options(int argc, char **argv, const struct option *longopts) {
    int longindex;
    while (1) {
        switch (getopt_long(argc, argv, "P:AONvh", longopts, &longindex)) {
            case 0:
                * (char**) longopts[longindex].flag = optarg;
                break;
            case 'P':
                break;
            case 'A':
                magic &= ~3;
                break;
            case 'O':
                magic |= 3;
                break;
            case 'N':
                magic ^= 4;
                break;
            case 'v':
                fprintf(stdout, "version\n");
                return 1;
            case 'h':
                fprintf(stdout, "help\n");
                return 1;
            case -1:
                if (magic&4) magic ^= 6;
                return 0;
            default:
                return 1;
        }
    }
}


int main(int argc, char **argv) {

    rp = rpload(argc, argv, get_plugins_path(argc, argv));

    if (parse_options(argc, argv, rp.opts)) {
        rpclose(rp);
        return 1;
    }

    nrftw((optind != argc) ? argv[optind] : "." , &walk_func);

    rpclose(rp);
    return 0;
}

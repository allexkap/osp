#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <getopt.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "nrftw.h"
#include "plugin_api.h"
#include "plugin_handler.h"



int or = 0, not = 0;
struct required_plugins rp;

int run_rp(const char *path) {
    int ok = 0, r = 0;
    for (int i = 0; i < rp.dls_len; ++i)
        if (ok += r = (*rp.ppfs[i])(path, &rp.opts[rp.opts_pos[i]], rp.opts_pos[i+1] - rp.opts_pos[i]), r < 0) return r;
    return not ^ (ok&&ok-rp.dls_len?or:!!ok);
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
        if (!strcmp(argv[i], "-P"))
            return argv[i+1];
    int p = 0;
    for (int i = 0; argv[0][i]; ++i) if (argv[0][i] == '/') p = i+1;
    argv[0][p] = 0;
    return argv[0];
}


int parse_options(int argc, char **argv, struct option *longopts) {
    int longindex;
    while (1) {
        switch (getopt_long(argc, argv, "P:AONvh", longopts, &longindex)) {
            case 0:
                if (longopts[longindex].flag)
                    * (char**) longopts[longindex].flag = optarg;
                else
                    longopts[longindex].flag = (int*) optarg;
                break;
            case 'P':
                break;
            case 'A':
                or = 0;
                break;
            case 'O':
                or = 1;
                break;
            case 'N':
                not ^= 1;
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

    rp = rpload(argc, argv, get_plugins_path(argc, argv));

    if (parse_options(argc, argv, rp.opts)) {
        rpclose(rp);
        return 1;
    }

    if (optind == argc) {
        fprintf(stdout, "List of all available plugins:\n");
        rpclose(rp);
        rpclose(rpload(0, argv, get_plugins_path(argc, argv)));
        return 0;
    }

    for (int i = 0; i < rp.opts_pos[rp.dls_len]; ++i)
        if (!rp.opts[i].flag) rp.opts[i].flag = (int*) "";

    nrftw(argv[optind], &walk_func);

    rpclose(rp);
    return 0;
}

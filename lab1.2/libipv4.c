#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "plugin_api.h"

static char *arg = NULL, argbin[4] = {};
static size_t arglen = 0;
static int state = 0;


int plugin_get_info(struct plugin_info* ppi) {
    static struct plugin_option sup_opts[] = {
        {{"ipv4-addr", required_argument, (int*) &arg, 0}, "like x.x.x.x"},
    };
    ppi->plugin_purpose = "Check if the given IPv4 contains in a file in text or binary form";
    ppi->plugin_author = "allexkap";
    ppi->sup_opts_len = 1;
    ppi->sup_opts = sup_opts;
    return 0;
}


int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {

    if (!state) {
        for (size_t i = 0; !arg && i < in_opts_len; ++i)
            if (!strcmp(in_opts[i].name, "ipv4-addr"))
                arg = (char*)in_opts[i].flag;
        if (!arg) {
            fprintf(stderr, "Error parsing arguments for option --ipv4-addr: Argument not found\n");
            state = -1;
        }
        else {
            arglen = strlen(arg);
            long byte;
            char *ptr = arg, *endptr = NULL;
            for (state = 0; state < 4; ++state) {
                byte = strtol(ptr, &endptr, 10);
                if (byte > 255 || byte < 0 || ptr == endptr || (endptr - ptr > 1 && *ptr == '0')) break;
                ptr = endptr + (*endptr == '.');
                argbin[state] = byte;
            }
            if (state != 4 || *endptr) {
                fprintf(stderr, "Error parsing arguments for option --ipv4-addr: %s\n", arg);
                state = -1;
            }
        }
    }
    if (state < 0) return -1;


    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error opening file %s\n", fname);
        return -1;
    }

    int exit_code = -1;
    char *ptr = NULL;

    struct stat st;
    int res = fstat(fd, &st);
    if (res < 0) {
        goto end;
    }

    ptr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        goto end;
    }

    for (int i = 0; i < st.st_size; ++i) {
        if (!strncmp(ptr+i, arg, arglen) || !strncmp(ptr+i, argbin, 4)) {
            exit_code = 0;
            goto end;
        }
    }
    exit_code = 1;

    end:
    close(fd);
    if (ptr && ptr != MAP_FAILED) munmap(ptr, st.st_size);
    if (exit_code < 0) fprintf(stderr, "Unknown error with file %s\n", fname);
    return exit_code;
}

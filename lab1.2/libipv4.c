#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "plugin_api.h"

static char *arg = NULL;
static size_t arglen = 0;
static uint32_t ipv4_bin = 0;
static int state = 0;


int plugin_get_info(struct plugin_info* ppi) {
    static struct plugin_option sup_opts[] = {
        {{"ipv4-addr", required_argument, (int*) &arg, 0}, "description"},
    };
    ppi->plugin_purpose = "purpose";
    ppi->plugin_author = "author";
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
                if (byte > 255 || byte < 0 || ptr == endptr || ptr - endptr > 3) break;
                ptr = endptr + (*endptr == '.');
                ipv4_bin <<= 8;
                ipv4_bin += byte;
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
        return -1;
    }

    int exit_code = -1;
    char *ptr = NULL;

    struct stat st;
    int res = fstat(fd, &st);
    if (res < 0) {
        goto end;
    }

    ptr = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        goto end;
    }

    for (int i = 0; i < st.st_size; ++i) {
        if (!strncmp(ptr+i, arg, arglen) || (st.st_size-i >= 4 && *(uint32_t*)(ptr+i) == ipv4_bin)) {
            exit_code = 0;
            goto end;
        }
    }
    exit_code = 1;

    end:
    close(fd);
    munmap(ptr, st.st_size);
    return exit_code;
}

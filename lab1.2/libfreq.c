#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "plugin_api.h"

static char *arg = NULL;
static long byte = -1;


int plugin_get_info(struct plugin_info* ppi) {
    static struct plugin_option sup_opts[] = {
        {{"freq-byte", required_argument, (int*) &arg, 0}, "value in bin, dec or hex"},
    };
    ppi->plugin_purpose = "Check if the given byte is the most frequent";
    ppi->plugin_author = "allexkap";
    ppi->sup_opts_len = 1;
    ppi->sup_opts = sup_opts;
    return 0;
}


int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {

    if (byte == -1) {
        for (size_t i = 0; !arg && i < in_opts_len; ++i)
            if (!strcmp(in_opts[i].name, "freq-byte"))
                arg = (char*)in_opts[i].flag;
        if (!arg) {
            fprintf(stderr, "Error parsing arguments for option --freq-byte: Argument not found\n");
            byte = -2;
        }
        else {
            int base = 10;
            if (!strncmp(arg, "0b", 2)) base = 2;
            if (!strncmp(arg, "0x", 2)) base = 16;

            char *endptr;
            byte = strtol(base!=10 ? arg+2 : arg, &endptr, base);

            if (!*arg || *endptr || byte > 255 || byte < 0) {
                fprintf(stderr, "Error parsing arguments for option --freq-byte: %s\n", arg);
                byte = -2;
            }
        }
    }
    if (byte < 0) return -1;

    FILE *file = fopen(fname, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", fname);
        return -1;
    }

    int bytes[256] = {};
    while (!feof(file)) ++bytes[getc(file)];

    fclose(file);

    for (int i = 0; i < 256; ++i) if (bytes[byte] < bytes[i] && byte != i) return 1;
    return 0;
}

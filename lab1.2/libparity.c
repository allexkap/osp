#include <stdio.h>
#include <string.h>
#include "plugin_api.h"

static char *arg = NULL;
static char mode = -1;


int plugin_get_info(struct plugin_info* ppi) {
    static struct plugin_option sup_opts[] = {
        {{"parity", required_argument, (int*) &arg, 0}, "description"},
    };
    ppi->plugin_purpose = "purpose";
    ppi->plugin_author = "author";
    ppi->sup_opts_len = 1;
    ppi->sup_opts = sup_opts;
    return 0;
}


int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {

    if (mode == -1) {
        for (size_t i = 0; !arg && i < in_opts_len; ++i)
            if (!strcmp(in_opts[i].name, "parity"))
                arg = (char*)in_opts[i].flag;
        if (!arg) {
            fprintf(stderr, "Error parsing arguments for option --parity: Argument not found\n");
            mode = -2;
        }
        else {
            mode = !strcmp(arg, "evens") << 2 | !strcmp(arg, "odds") << 1 | !strcmp(arg, "eq");
            if (!mode) {
                fprintf(stderr, "Error parsing arguments for option --parity: %s\n", arg);
                mode = -2;
            }
        }
    }
    if (mode < 0) return -1;

    FILE *file = fopen(fname, "r");
    if (!file) {
        fprintf(stderr, "Error opening file %s\n", fname);
        return -1;
    }

    int odds = 0, evens = 0;
    while (!feof(file)) (getc(file) % 2) ? ++odds : ++evens;

    fclose(file);

    switch(mode) {
        case 1:
            return odds == evens;
        case 2:
            return odds > evens;
        case 4:
            return odds < evens;
        default:
            return -1;
    }
}

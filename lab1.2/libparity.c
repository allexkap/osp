#include <stdio.h>
#include <string.h>
#include "plugin_api.h"

char *arg = NULL;
char mode = -1;


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

    (void) in_opts, (void) in_opts_len; // suppress unused parameter

    if (mode < 0) mode = !strcmp(arg, "evens") << 2 | !strcmp(arg, "odds") << 1 | !strcmp(arg, "eq");

    FILE *file = fopen(fname, "r");
    if (!file) return -1;

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

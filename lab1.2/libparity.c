#include "plugin_api.h"

char *arg;

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

#include <stdio.h>
int plugin_process_file(const char *fname, struct option in_opts[], size_t in_opts_len) {
    (void) fname, (void) in_opts, (void) in_opts_len;   // suppress unused parameter
    printf("%s\n", arg);
    return 0;
}

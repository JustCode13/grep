#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// static bool is_option_c = false;
// static bool is_option_i = false;
// static bool is_option_n = false;
// static bool is_option_v = false;
// static bool is_option_r = false;

int validate_options(char *opt_arg);

int main(int argc, char *argv[]) {

    size_t file_count = argc - 2;

    if (argc < 3) {
        printf("Usage: %s [-options] [-pattern] [-files]\n", argv[0]);
        return 1;
    }

    bool is_options = false;

    if (argv[1][0] == '-') {
        is_options = true;
        file_count -= 1;

        if (validate_options(argv[1]) != 0) {
            return 1;
        }
    }

    if (file_count == 0) {
        printf("No file provided\n");
    }

    if (is_options) {
        printf("Hello\n");
    }

    return 0;
}

int validate_options(char *opt_arg) {
    size_t options_len = strlen(opt_arg) - 1;

    for (size_t i = 0; i < options_len; i++) {
        if (!strchr("icnvr", opt_arg[i + 1])) {
            printf("Invalid option: %c\n", opt_arg[i + 1]);
            return 1;
        }

        // switch (opt_arg[i + 1]) {
        // case 'i':
        //     is_option_i = true;
        // case 'c':
        //     is_option_c = true;
        // case 'n':
        //     is_option_n = true;
        // case 'v':
        //     is_option_v = true;
        // case 'r':
        //     is_option_r = true;
        // }
    }

    for (size_t i = 0; i < options_len; i++) {
        printf("option: %c\n", opt_arg[i + 1]);
    }

    return 0;
}

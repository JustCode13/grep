#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// static bool is_option_c = false;
// static bool is_option_i = false;
// static bool is_option_n = false;
// static bool is_option_v = false;
// static bool is_option_r = false;

#define MAX_LINE_LENGTH 256

int validate_options(char *opt_arg);

int open_file(char *file_name);

int store_validate_file_names_and_fds(char *file_names[], int file_fds[],
                                      size_t file_count, char *argv[],
                                      size_t skip_arg, bool is_options);

int read_line(int file_fd, char *buf);

int main(int argc, char *argv[]) {

    size_t file_count = (size_t)argc - 2;
    size_t skip_arg = 2;

    if (argc < 3) {
        printf("Usage: %s [-options] [-pattern] [-files]\n", argv[0]);
        return 1;
    }

    bool is_options = false;

    if (argv[1][0] == '-') {
        is_options = true;
        file_count -= 1;
        skip_arg += 1;

        if (validate_options(argv[1]) != 0) {
            return 1;
        }
    }

    if (file_count == 0) {
        printf("Error: No file provided\n");
    }

    char *file_names[file_count];
    int file_fds[file_count];

    if (store_validate_file_names_and_fds(file_names, file_fds, file_count,
                                          argv, skip_arg, is_options) != 0) {
        return 1;
    }

    is_options ? printf("options are there\n")
               : printf("options are not there\n");

    char line[MAX_LINE_LENGTH];

    for (size_t i = 0; i < file_count; i++) {
        if (read_line(file_fds[i], line) != 0) {
            return 1;
        }

        printf("%s\n", line);
    }

    return 0;
}

int read_line(int file_fd, char *buf) {
    if (read(file_fd, buf, MAX_LINE_LENGTH - 1) < 1) {
        printf("Error: can't read line");
        return 1;
    }

    return 0;
}

int store_validate_file_names_and_fds(char *file_names[], int file_fds[],
                                      size_t file_count, char *argv[],
                                      size_t skip_arg, bool is_options) {
    if (is_options) {
        if (open(argv[2], O_RDONLY) != -1) {
            printf("Error: No pattern provided\n");
            return 1;
        }
    } else {
        if (open(argv[1], O_RDONLY) != -1) {
            printf("Error: No pattern provided\n");
            return 1;
        }
    }
    for (size_t i = 0; i < file_count; i++) {
        file_names[i] = argv[skip_arg + i];
        file_fds[i] = open_file(file_names[i]);

        if (file_fds[i] == -1) {
            return 1;
        }
    }

    // for (size_t i = 0; i < file_count; i++) {
    //     printf("%s\n", file_names[i]);
    //     printf("%d\n", file_fds[i]);
    // }

    return 0;
}

int open_file(char *file_name) {
    int fd = open(file_name, O_RDONLY);

    if (fd == -1) {
        printf("Error: opening file: %s\n", file_name);
        return -1;
    }

    return fd;
}

int validate_options(char *opt_arg) {
    size_t options_len = strlen(opt_arg) - 1;

    for (size_t i = 0; i < options_len; i++) {
        if (!strchr("icnvr", opt_arg[i + 1])) {
            printf("Error: Invalid option: %c\n", opt_arg[i + 1]);
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

    // for (size_t i = 0; i < options_len; i++) {
    //     printf("option: %c\n", opt_arg[i + 1]);
    // }

    return 0;
}

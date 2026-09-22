#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool is_option_c = false;
static bool is_option_i = false;
static bool is_option_n = false;
static bool is_option_v = false;
static bool is_option_r = false;

#define MAX_LINE_LENGTH 1024

int validate_options(char *opt_arg);

int open_file(char *file_name);

int store_validate_file_names_and_fds(char *file_names[], int file_fds[],
                                      size_t file_count, char *argv[],
                                      size_t skip_arg, bool is_options);

int read_line(int file_fd, char *buf);

int read_files_by_line(size_t file_count, int *file_fds, char *pattern,
                       bool is_options);

int close_files(char *file_names[], int *file_fds, size_t file_count);

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

    char *pattern = is_options ? argv[2] : argv[1];

    if (store_validate_file_names_and_fds(file_names, file_fds, file_count,
                                          argv, skip_arg, is_options) != 0) {
        return 1;
    }

    is_options ? printf("options are there\n")
               : printf("options are not there\n");

    if (read_files_by_line(file_count, file_fds, pattern, is_options) != 0) {
        return 1;
    }

    if (close_files(file_names, file_fds, file_count) != 0) {
        return 1;
    }

    return 0;
}

int close_files(char *file_names[], int *file_fds, size_t file_count) {
    for (size_t i = 0; i < file_count; i++) {
        if (close(file_fds[i]) != 0) {
            printf("Error: closing file %s\n", file_names[i]);
            return 1;
        }
    }

    return 0;
}

int read_files_by_line(size_t file_count, int *file_fds, char *pattern,
                       bool is_options) {
    char buffer[4056];
    char line[MAX_LINE_LENGTH];
    size_t line_length;

    for (size_t i = 0; i < file_count; i++) {
        bool is_line_start = true;
        size_t line_number = 0;
        ssize_t bytes_read;
        int match_count;

        line_length = 0;

        while ((bytes_read = read(file_fds[i], buffer, sizeof(buffer) - 2)) >
               0) {

            buffer[bytes_read] = '\n';
            buffer[bytes_read + 1] = '\0';
            match_count = 0;

            for (size_t j = 0; j < (size_t)bytes_read; j++) {
                if (buffer[j] == '\n' || buffer[j] == '\0') {

                    line_number++;

                    if (line_length >= MAX_LINE_LENGTH - 1) {
                        printf("Error: Line exceeds maximum line_length\n");
                    }

                    line[line_length] = '\n';
                    line_length++;

                    line[line_length] = '\0';

                    if (strstr(line, pattern) != NULL) {
                        // if no options are given
                        if (!is_options) {
                            printf("%s\n", line);

                            // if just option a is given
                        } else if (is_line_start && is_option_n) {
                            char number[32];

                            int len = snprintf(number, sizeof(number),
                                               "%zu: ", line_number);

                            if (len < 0) {
                                printf("Error formatting line number\n");
                                return 1;
                            }

                            if (write(STDOUT_FILENO, number, sizeof(number)) ==
                                -1) {
                                perror("write");
                                return 1;
                            }

                            printf("%s\n", line);
                        } else if (is_option_c) {
                            match_count++;
                        }
                    }

                    line_length = 0;
                } else {
                    if (line_length >= MAX_LINE_LENGTH - 1) {
                        printf("Error Exceedng line size\n");

                        return 1;
                    }

                    line[line_length] = buffer[j];
                    line_length++;
                }
            }
        }

        if (is_option_c) {
            printf("%d\n", match_count);
        }

        if (bytes_read == -1) {
            perror("read");
            return 1;
        }

        /*
         * Handle a final line that doesn't end with '\n'.
         */
        // if (line_length > 0) {
        //
        //     line[line_length] = '\0';
        //
        //     if (strstr(line, pattern) != NULL) {
        //         printf("%d", is_options);
        //         if (!is_options) {
        //
        //             printf("%s\n", line);
        //         }
        //     }
        // }
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

        switch (opt_arg[i + 1]) {
        case 'i':
            is_option_i = true;
            break;
        case 'c':
            is_option_c = true;
            break;
        case 'n':
            is_option_n = true;
            break;
        case 'v':
            is_option_v = true;
            break;
        case 'r':
            is_option_r = true;
            break;
        default:
            return 1;
            break;
        }
    }

    // for (size_t i = 0; i < options_len; i++) {
    //     printf("option: %c\n", opt_arg[i + 1]);
    // }

    return 0;
}

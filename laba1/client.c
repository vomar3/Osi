#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void delete(char *str) {
    if (str == NULL) {
        return;
    }

    char vowels[] = "AEIOUYaeiouy";
    size_t len = strlen(str);
    size_t j = 0;

    for (size_t i = 0; i < len; ++i) {
        if (strchr(vowels, str[i]) == NULL) {
            str[j++] = str[i];
        }
    }

    str[j] = '\0';
}

int main(int argc, char **argv)
{
    char buf[4096];
    ssize_t bytes;

    pid_t pid = getpid();

    // NOTE: `O_WRONLY` only enables file for writing
    // NOTE: `O_CREAT` creates the requested file if absent
    // NOTE: `O_TRUNC` empties the file prior to opening
    // NOTE: `O_APPEND` subsequent writes are being appended instead of overwritten
    int32_t file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0600);
    if (file == -1)
    {
        const char msg[] = "error: failed to open requested file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }

    while (bytes = read(STDIN_FILENO, buf, sizeof(buf)))
    {
        if (bytes < 0)
        {
            const char msg[] = "error: failed to read from stdin\n";
            write(STDERR_FILENO, msg, sizeof(msg));
            exit(EXIT_FAILURE);
        }
        else if (buf[0] == '\n')
        {
            break;
        }

        {
            buf[bytes - 1] = '\0';
            delete(buf);
            size_t len = strlen(buf);
            buf[len] = '\n';
            int32_t written = write(file, buf, len + 1);
            if (written != len + 1)
            {
                const char msg[] = "error: client failed to write to file\n";
                write(STDERR_FILENO, msg, sizeof(msg));
                exit(EXIT_FAILURE);
            }
        }
    }
    if (close(file) == -1)
    {
        const char msg[] = "error: client failed to close file\n";
        write(STDERR_FILENO, msg, sizeof(msg));
        exit(EXIT_FAILURE);
    }
    return 0;
}
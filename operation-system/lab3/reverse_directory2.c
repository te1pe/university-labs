#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 4096

void ReverseString(char *string) {
    int length = strlen(string);
    for (int i = 0; i < length / 2; i++) {
        char temp = string[i];
        string[i] = string[length - i - 1];
        string[length - i - 1] = temp;
    }
}

static ssize_t WriteAll(int fd, const char *buf, size_t count) {
    size_t total_written = 0;
    while (total_written < count) {
        ssize_t n = write(fd, buf + total_written, count - total_written);
        if (n == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        total_written += (size_t)n;
    }
    return (ssize_t)total_written;
}

static ssize_t ReadAll(int fd, char *buf, size_t count) {
    size_t total_read = 0;
    while (total_read < count) {
        ssize_t n = read(fd, buf + total_read, count - total_read);
        if (n == -1) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (n == 0) break;
        total_read += (size_t)n;
    }
    return (ssize_t)total_read;
}

static void ReverseFileContent(const char *src_path, const char *dst_path) {
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        perror("open src");
        return;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd == -1) {
        perror("open dst");
        close(src_fd);
        return;
    }

    off_t file_size = lseek(src_fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
        perror("lseek SEEK_END");
        close(src_fd);
        close(dst_fd);
        return;
    }

    char buffer[BUFFER_SIZE];
    off_t remaining = file_size;

    while (remaining > 0) {
        off_t chunk_size = (remaining >= (off_t)BUFFER_SIZE)
                            ? (off_t)BUFFER_SIZE
                            : remaining;
        remaining -= chunk_size;

        if (lseek(src_fd, remaining, SEEK_SET) == (off_t)-1) {
            perror("lseek SEEK_SET");
            break;
        }

        ssize_t bytes_read = ReadAll(src_fd, buffer, (size_t)chunk_size);
        if (bytes_read == -1) {
            perror("read");
            break;
        }

        for (ssize_t i = 0; i < bytes_read / 2; i++) {
            char tmp = buffer[i];
            buffer[i] = buffer[bytes_read - 1 - i];
            buffer[bytes_read - 1 - i] = tmp;
        }

        if (WriteAll(dst_fd, buffer, (size_t)bytes_read) == -1) {
            perror("write");
            break;
        }
    }

    close(src_fd);
    close(dst_fd);
}

void CopyReverseCatalog(const char *src_dir) {
    char dst_dir[256];

    if (strlen(src_dir) >= sizeof(dst_dir)) {
        fprintf(stderr, "Ошибка: путь к каталогу слишком длинный\n");
        return;
    }

    strcpy(dst_dir, src_dir);
    ReverseString(dst_dir);

    if (mkdir(dst_dir, 0755) == -1 && errno != EEXIST) {
        perror("mkdir");
        return;
    }

    DIR *dir = opendir(src_dir);
    if (!dir) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    errno = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) {
            errno = 0;
            continue;
        }

        char reversed_name[256];
        strncpy(reversed_name, entry->d_name, sizeof(reversed_name) - 1);
        reversed_name[sizeof(reversed_name) - 1] = '\0';
        ReverseString(reversed_name);

        char src_file_path[512];
        char dst_file_path[512];

        int ret = snprintf(src_file_path, sizeof(src_file_path),
                           "%s/%s", src_dir, entry->d_name);
        if (ret < 0 || ret >= (int)sizeof(src_file_path)) {
            fprintf(stderr, "Путь слишком длинный: %s/%s\n", src_dir, entry->d_name);
            errno = 0;
            continue;
        }

        ret = snprintf(dst_file_path, sizeof(dst_file_path),
                       "%s/%s", dst_dir, reversed_name);
        if (ret < 0 || ret >= (int)sizeof(dst_file_path)) {
            fprintf(stderr, "Путь слишком длинный: %s/%s\n", dst_dir, reversed_name);
            errno = 0;
            continue;
        }

        ReverseFileContent(src_file_path, dst_file_path);
        errno = 0;
    }

    if (errno != 0) {
        perror("readdir");
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    CopyReverseCatalog(argv[1]);
    return EXIT_SUCCESS;
}

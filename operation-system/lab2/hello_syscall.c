#include <unistd.h>
#include <sys/syscall.h>

long my_write(int fd, const void *buf, size_t count) {
    return syscall(SYS_write, fd, buf, count);
}

int main() {
    my_write(1, "hello world\n", 12);
    return 0;
}


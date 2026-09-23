#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/mman.h>
#include <sys/wait.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

static unsigned int *g_buf = NULL;
static size_t        g_buf_bytes = 0;
static pid_t         g_child_pid = 0;

static void on_sigint(int sig)
{
    (void)sig;
    static const char msg[] = "\n[main] получен SIGINT, завершаюсь\n";
    (void)!write(STDERR_FILENO, msg, sizeof(msg) - 1);
    if (g_child_pid > 0)
        kill(g_child_pid, SIGTERM);
    if (g_buf && g_buf_bytes)
        munmap(g_buf, g_buf_bytes);
    _exit(0);
}

int main(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) { perror("sysconf"); return 1; }
    size_t bytes = (size_t)page_size;
    size_t ncells = bytes / sizeof(unsigned int);

    unsigned int *buf = mmap(NULL, bytes,
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS,
                              -1, 0);
    if (buf == MAP_FAILED) { perror("mmap"); return 1; }

    for (size_t i = 0; i < ncells; i++)
        buf[i] = (unsigned int)i;

    g_buf = buf;
    g_buf_bytes = bytes;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); munmap(buf, bytes); return 1; }

    if (pid == 0) {
        g_child_pid = 0;
        size_t mismatches = 0, checks = 0;
        unsigned long long report_iter = 0;
        while (1) {
            for (size_t i = 0; i < ncells - 1; i++) {
                unsigned int a = buf[i];
                unsigned int b = buf[i + 1];
                if (b != a + 1) {
                    mismatches++;
                    if (mismatches <= 5 || mismatches % 100000 == 0) {
                        fprintf(stderr,
                            "[READER pid=%d] mismatch #%zu at cell=%zu: "
                            "buf[i]=%u, buf[i+1]=%u (diff=%d)\n",
                            getpid(), mismatches, i, a, b, (int)(b - a));
                    }
                }
                checks++;
            }
            if (++report_iter % 5000 == 0) {
                fprintf(stderr,
                    "[READER pid=%d] iter=%llu  checks=%zu  mismatches=%zu\n",
                    getpid(), report_iter, checks, mismatches);
            }
        }
    } else {
        g_child_pid = pid;
        unsigned int value = 0;
        while (1) {
            for (size_t i = 0; i < ncells; i++)
                buf[i] = value++;
        }
    }
    return 0;
}

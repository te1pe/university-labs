#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

static pid_t g_child_pid = 0;

static void on_sigint(int sig)
{
    (void)sig;
    static const char m[] = "\n[main] SIGINT, корректное завершение\n";
    (void)!write(STDERR_FILENO, m, sizeof(m) - 1);
    if (g_child_pid > 0) kill(g_child_pid, SIGTERM);
    _exit(0);
}

int main(void)
{
    int pfd[2];
    if (pipe(pfd) < 0) { perror("pipe"); return 1; }

    struct sigaction sa = {0};
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); close(pfd[0]); close(pfd[1]); return 1; }

    if (pid == 0) {
        close(pfd[1]);
        unsigned int expected = 0, got = 0;
        size_t count = 0, mismatches = 0;
        while (1) {
            ssize_t n = read(pfd[0], &got, sizeof(got));
            if (n == 0) { fprintf(stderr, "[READER] EOF\n"); break; }
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("read"); break;
            }
            if (n != (ssize_t)sizeof(got)) {
                size_t total = (size_t)n;
                char *p = (char *)&got;
                while (total < sizeof(got)) {
                    ssize_t k = read(pfd[0], p + total, sizeof(got) - total);
                    if (k <= 0) break;
                    total += (size_t)k;
                }
            }
            if (got != expected) {
                mismatches++;
                if (mismatches <= 10) {
                    fprintf(stderr,
                        "[READER] mismatch #%zu: expected=%u got=%u\n",
                        mismatches, expected, got);
                }
                expected = got;
            }
            expected++; count++;
            if ((count & 0x3FFFFF) == 0) {
                fprintf(stderr,
                    "[READER] прочитано=%zu последнее=%u mismatches=%zu\n",
                    count, got, mismatches);
            }
        }
        close(pfd[0]);
        return 0;
    } else {
        g_child_pid = pid;
        close(pfd[0]);
        signal(SIGPIPE, SIG_IGN);

        unsigned int value = 0;
        while (1) {
            ssize_t n = write(pfd[1], &value, sizeof(value));
            if (n < 0) {
                if (errno == EINTR) continue;
                if (errno == EPIPE) {
                    fprintf(stderr, "[WRITER] читатель закрыл pipe, выходим\n");
                    break;
                }
                perror("write"); break;
            }
            if (n != (ssize_t)sizeof(value)) {
                size_t total = (size_t)n;
                const char *p = (const char *)&value + n;
                while (total < sizeof(value)) {
                    ssize_t k = write(pfd[1], p, sizeof(value) - total);
                    if (k <= 0) break;
                    total += (size_t)k; p += k;
                }
            }
            value++;
        }
        close(pfd[1]);
        waitpid(pid, NULL, 0);
        return 0;
    }
}

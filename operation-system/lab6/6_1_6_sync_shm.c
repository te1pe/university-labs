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

static volatile sig_atomic_t g_data_ready    = 0;
static volatile sig_atomic_t g_self_is_writer = 0;
static volatile sig_atomic_t g_peer_pid       = 0;
static unsigned int         *g_buf = NULL;
static size_t                g_buf_bytes = 0;

static void on_sigusr(int sig)
{
    (void)sig;
    g_data_ready = 1;
}

static void on_sigint(int sig)
{
    (void)sig;
    if (g_self_is_writer) {
        static const char m[] = "[WRITER] получил SIGINT, завершаюсь, отдаю ресурсы\n";
        (void)!write(STDERR_FILENO, m, sizeof(m) - 1);
    } else {
        static const char m[] = "[READER] получил SIGINT, завершаюсь, отдаю ресурсы\n";
        (void)!write(STDERR_FILENO, m, sizeof(m) - 1);
    }
    if (g_peer_pid > 0)
        kill(g_peer_pid, SIGTERM);
    if (g_buf && g_buf_bytes)
        munmap(g_buf, g_buf_bytes);
    _exit(0);
}

static void on_sigterm(int sig)
{
    (void)sig;
    if (g_self_is_writer) {
        static const char m[] = "[WRITER] завершаюсь, потому что READER остановлен\n";
        (void)!write(STDERR_FILENO, m, sizeof(m) - 1);
    } else {
        static const char m[] = "[READER] завершаюсь, потому что WRITER остановлен\n";
        (void)!write(STDERR_FILENO, m, sizeof(m) - 1);
    }
    if (g_buf && g_buf_bytes)
        munmap(g_buf, g_buf_bytes);
    _exit(0);
}

static void install_handler(int signo, void (*h)(int))
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = h;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(signo, &sa, NULL) < 0) { perror("sigaction"); _exit(1); }
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
    g_buf = buf;
    g_buf_bytes = bytes;

    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) {
        perror("sigprocmask"); munmap(buf, bytes); return 1;
    }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); munmap(buf, bytes); return 1; }

    if (pid == 0) {
        g_self_is_writer = 0;
        g_peer_pid       = getppid();
        install_handler(SIGUSR1, on_sigusr);
        install_handler(SIGINT,  on_sigint);
        install_handler(SIGTERM, on_sigterm);
        fprintf(stderr, "[READER pid=%d] стартовал, peer=%d, ожидаю данные\n",
                getpid(), (int)g_peer_pid);

        size_t cycle = 0, mismatches = 0;
        while (1) {
            while (!g_data_ready) sigsuspend(&oldmask);
            g_data_ready = 0;

            int ok = 1;
            for (size_t i = 0; i < ncells - 1; i++) {
                if (buf[i + 1] != buf[i] + 1) {
                    mismatches++; ok = 0;
                    if (mismatches <= 5) {
                        fprintf(stderr,
                            "[READER] ВНЕЗАПНО mismatch на cell=%zu: %u → %u\n",
                            i, buf[i], buf[i + 1]);
                    }
                    break;
                }
            }
            cycle++;
            if (cycle % 100000 == 0) {
                fprintf(stderr,
                    "[READER] cycle=%zu ok=%d first=%u last=%u mismatches=%zu\n",
                    cycle, ok, buf[0], buf[ncells - 1], mismatches);
            }
            if (kill(g_peer_pid, SIGUSR2) < 0) { perror("kill(SIGUSR2)"); break; }
        }
        munmap(buf, bytes);
        return 0;
    } else {
        g_self_is_writer = 1;
        g_peer_pid       = pid;
        install_handler(SIGUSR2, on_sigusr);
        install_handler(SIGINT,  on_sigint);
        install_handler(SIGTERM, on_sigterm);
        fprintf(stderr, "[WRITER pid=%d] стартовал, peer=%d, начинаю работу\n",
                getpid(), (int)pid);

        unsigned int value = 0;
        size_t cycle = 0;
        while (1) {
            for (size_t i = 0; i < ncells; i++) buf[i] = value++;
            g_data_ready = 0;
            if (kill(g_peer_pid, SIGUSR1) < 0) { perror("kill(SIGUSR1)"); break; }
            while (!g_data_ready) sigsuspend(&oldmask);
            cycle++;
            if (cycle % 100000 == 0) {
                fprintf(stderr, "[WRITER] cycle=%zu value=%u\n", cycle, value);
            }
        }
        munmap(buf, bytes);
        return 0;
    }
}

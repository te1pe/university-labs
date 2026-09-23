#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

int global_var = 42;

volatile sig_atomic_t got_sighup = 0;

void hup_handler(int signo) {
    (void)signo;
    got_sighup = 1;
    const char msg[] = "[handler] поймал SIGHUP\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
}

int main() {
    int local_var = 10;

    struct sigaction sa;
    sa.sa_handler = hup_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Родительский PID: %d (global=%d, local=%d)\n",
           getpid(), global_var, local_var);

    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    if (pid == 0) {
        printf("Дочерний PID: %d, PPID: %d\n", getpid(), getppid());
        for (int i = 0; i < 12; i++) {
            sleep(5);
            if (got_sighup) {
                printf("Дочерний %d ВЫЖИЛ после SIGHUP\n", getpid());
                got_sighup = 0;
            }
        }
        exit(5);
    } else {
        for (int i = 0; i < 12; i++) {
            sleep(5);
            if (got_sighup) {
                printf("Родитель %d ВЫЖИЛ после SIGHUP\n", getpid());
                got_sighup = 0;
            }
        }
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
            printf("Дочерний завершился с кодом: %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("Дочерний убит сигналом: %d\n", WTERMSIG(status));
    }
    return 0;
}

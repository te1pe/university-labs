#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

static pid_t spawn_with_parent_stdout(const char *path, char *const argv[])
{
    int pfd[2];
    if (pipe(pfd) < 0) { perror("pipe"); return -1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); close(pfd[0]); close(pfd[1]); return -1; }

    if (pid == 0) {
        close(pfd[1]);
        if (dup2(pfd[0], STDIN_FILENO) < 0) { perror("dup2 child"); _exit(127); }
        close(pfd[0]);
        execv(path, argv);
        perror("execv");
        _exit(127);
    }

    close(pfd[0]);
    if (dup2(pfd[1], STDOUT_FILENO) < 0) {
        perror("dup2 parent"); close(pfd[1]); return -1;
    }
    close(pfd[1]);
    setvbuf(stdout, NULL, _IOLBF, 0);
    return pid;
}

int main(void)
{
    fprintf(stderr, "[parent stderr] Сейчас перенаправлю stdout в stdin cat\n");

    char *args[] = { (char *)"cat", NULL };
    pid_t child = spawn_with_parent_stdout("/bin/cat", args);
    if (child < 0) { fprintf(stderr, "spawn failed\n"); return 1; }

    for (int i = 0; i < 5; i++) {
        printf("[from parent via pipe] line #%d, pid=%d\n", i, getpid());
        fflush(stdout);
    }
    fclose(stdout);

    int status = 0;
    if (waitpid(child, &status, 0) < 0) { perror("waitpid"); return 1; }

    if (WIFEXITED(status))
        fprintf(stderr, "[parent stderr] cat завершился с кодом %d\n",
                WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        fprintf(stderr, "[parent stderr] cat убит сигналом %d\n",
                WTERMSIG(status));
    return 0;
}

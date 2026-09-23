#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int global_not_inited_var;
int global_inited_var = 1;
const int global_const_var = 2;

int* InitLocalVar() {
     int local_inited_var = 4;
    printf("local_inited_var:        %d\n", local_inited_var);
    return &local_inited_var;
}

void InitBufferInHeap() {
    int buffer_size = 100;

    char *local_buffer = malloc(buffer_size * sizeof(char));
    if (local_buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(local_buffer, "Hello, World!");
    printf("local_buffer:           %s, %p\n", local_buffer, (void*)local_buffer);

    free(local_buffer);

    /*
       После free нельзя читать local_buffer как строку.
       Поэтому НЕ делаем:
       printf("%s", local_buffer);
    */
    printf("local_buffer after free: %p\n", (void*)local_buffer);

    local_buffer = malloc(buffer_size * sizeof(char));
    if (local_buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(local_buffer, "Hello, World!");
    printf("local_buffer:           %s, %p\n", local_buffer, (void*)local_buffer);

    char *mid_buffer = local_buffer + 50;
    printf("mid_buffer:             %p\n", (void*)mid_buffer);

    /*
       Нельзя:
       free(mid_buffer);

       Нужно освобождать именно тот адрес, который вернул malloc.
    */
    free(local_buffer);

    local_buffer = NULL;
}

int main() {
    int local_var = 0;
    static int static_var;
    const int local_const_var = 3;

    printf("pid: %d\n\n", getpid());

    printf("global_not_inited_var:  %p, %d\n", (void*)&global_not_inited_var, global_not_inited_var);
    printf("global_inited_var:      %p, %d\n", (void*)&global_inited_var, global_inited_var);
    printf("global_const_var:       %p, %d\n", (void*)&global_const_var, global_const_var);
    printf("local_var:              %p, %d\n", (void*)&local_var, local_var);
    printf("static_var:             %p, %d\n", (void*)&static_var, static_var);
    printf("local_const_var:        %p, %d\n", (void*)&local_const_var, local_const_var);

    InitLocalVar();
    int *local_inited_var_address = InitLocalVar();
printf("вернувшийся указатель = %p\n", (void*)local_inited_var_address);
printf("разыменование: %d\n", *local_inited_var_address);

    printf("local_inited_var:       %p, %d\n",
           (void*)local_inited_var_address,
           *local_inited_var_address);

    InitBufferInHeap();

    sleep(60);

    return EXIT_SUCCESS;
}

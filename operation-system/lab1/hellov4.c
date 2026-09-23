#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

typedef void (*hello_func_t)(void);

int main(void) {
    printf("Hello world\n");

    void *handle = dlopen("./libruntimehello.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        return 1;
    }

    dlerror(); // очистить старую ошибку

    hello_func_t func = (hello_func_t)dlsym(handle, "hello_from_dyn_runtime_lib");
    char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "dlsym error: %s\n", error);
        dlclose(handle);
        return 1;
    }

    func();

    dlclose(handle);
    return 0;
}

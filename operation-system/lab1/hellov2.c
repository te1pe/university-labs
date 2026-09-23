#include <stdio.h>
#include "static_hello.h"

int main(void) {
    printf("Hello world\n");
    hello_from_static_lib();
    return 0;
}

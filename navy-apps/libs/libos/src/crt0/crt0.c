#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);

extern char **environ;

void call_main(uintptr_t *args) {
    char *empty[] = {NULL};
    int argc = *args;
    char **argv = malloc(argc * sizeof(char *));
    args++;
    for (int i = 0; i < argc; i++) {
        argv[i] = (char *) (*args);
        args++;
    }
    args++;
    environ = empty;
    exit(main(argc, argv, empty));
    assert(0);
}

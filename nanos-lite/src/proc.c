#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *, const char *);

void context_uload(PCB *start, const char *filename, char *const argv[], char *const envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;


void switch_boot_pcb() {
    current = &pcb_boot;
}

void hello_fun(void *arg) {
    int j = 1;
    while (1) {
        Log("Hello World from Nanos-lite with arg '%s' for the %dth time!", arg, j);
        j++;
        yield();
    }
}

void context_kload(PCB *start, void (*entry)(void *), void *argc) {
    Area ar;
    ar.start = start->stack;

    ar.end = (start->stack + sizeof(PCB));
    start->cp = kcontext(ar, entry, argc);

}


void init_proc() {
    char *argv[] = {NULL};
    char *empty[] = {NULL};
    context_uload(&pcb[0], "/bin/bird", argv, empty);
    context_kload(&pcb[1], hello_fun, "12");
    switch_boot_pcb();

    Log("Initializing processes...");

    // load program here
//    naive_uload(NULL, "/bin/exec-test");
}

Context *schedule(Context *prev) {
// save the context pointer
    current->cp = prev;
//    pcb[0]=*current;

// always select pcb[0] as the new process
//    current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
    current = &pcb[0];
//    printf("cp===%p\n",current->cp);
//    printf("-------\n");
// then return the new context
    return current->cp;
}

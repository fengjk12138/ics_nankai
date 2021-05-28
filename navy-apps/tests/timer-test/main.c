#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <NDL.h>

int main() {
    int old = NDL_GetTicks();
    while (1) {
        if (NDL_GetTicks() - old > 500) {
            printf("PASS!!!\n");
            old = NDL_GetTicks();
        }
    }
    return 0;
}

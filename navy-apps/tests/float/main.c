#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <fixedptc.h>

int main() {
    fixedpt a = fixedpt_rconst(1.2);
    fixedpt b = fixedpt_fromint(10);
    int c = 0;
    char ans[1000];
        fixedpt_str(
                fixedpt_div(fixedpt_mul(a + FIXEDPT_ONE, b), fixedpt_rconst(2.3))
                +
                        fixedpt_div(fixedpt_mul(fixedpt_floor(fixedpt_rconst(2)),fixedpt_abs(fixedpt_rconst(-2.3)))
               , fixedpt_ceil(fixedpt_rconst(-2.3)))
//                fixedpt_(fixedpt_rconst(-2.3))
                ,ans, 8);

    printf("%s\n",ans);
}

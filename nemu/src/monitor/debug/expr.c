#include <isa.h>
#include <assert.h>
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
    TK_NOTYPE = 256, TK_EQ, TK_BQ, TK_SQ, TK_S, TK_B, TK_REG, TK_HEX, TK_DEC

    /* TODO: Add more token types */

};

static struct rule {
    char *regex;
    int token_type;
} rules[] = {

        /* TODO: Add more rules.
         * Pay attention to the precedence level of different rules.
         */

        {" +",          TK_NOTYPE},    // spaces
        {"\\+",         '+'},         // plus
        {"==",          TK_EQ},        // equal
        {"-",           '-'},//sub
        {"\\*",         '*'},//mul
        {"/",           '/'},//div
        {"<=",          TK_SQ},
        {">=",          TK_BQ},
        {">",           TK_B},
        {"<",           TK_S},
        {"^(\\$)",      TK_REG},
        {"0x[0-9]*",    TK_HEX},
        {"\\(",         '('},
        {"\\)",         ')'},
        {"[1-9][0-9]*", TK_DEC},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
    int i;
    char error_msg[128];
    int ret;

    for (i = 0; i < NR_REGEX; i++) {
        ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
        if (ret != 0) {
            regerror(ret, &re[i], error_msg, 128);
            panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
        }
    }
}

typedef struct token {
    int type;
    char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
    int position = 0;
    int i;
    regmatch_t pmatch;

    nr_token = 0;

    while (e[position] != '\0') {
        /* Try all rules one by one. */
        for (i = 0; i < NR_REGEX; i++) {
            if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
                char *substr_start = e + position;
                int substr_len = pmatch.rm_eo;

                Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
                    i, rules[i].regex, position, substr_len, substr_len, substr_start);

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */

                tokens[nr_token].type = rules[i].token_type;
                for (int udp = 0; udp < substr_len; udp++)
                    tokens[nr_token].str[udp] = e[substr_len + position];
                tokens[nr_token].str[substr_len] = '\0';
                nr_token++;
                switch (rules[i].token_type) {
                    case TK_NOTYPE:
                        nr_token--;
                        break;
                    default:
                        TODO();
                }
                break;
            }
        }

        if (i == NR_REGEX) {
            printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
            return false;
        }
    }

    return true;
}

int check_parentheses(int l, int r, bool *success) {
    if (tokens[l].type == '(' && tokens[r].type == ')')
        return 1;
    else if (tokens[l].type == '(') {
        printf("Brackets should match. Miss right bracket.");
        *success = false;
    } else if (tokens[r].type == ')') {
        printf("Brackets should match. Miss left bracket.");
        *success = false;
    }
    return 0;
}

word_t eval(int l, int r, bool *success) {
    if (l > r) {
        printf("please check your expr.");
        *success = false;
    } else if (l == r) {
        if (tokens[l].type == TK_HEX) {
            return atoi(tokens[l].str);
        } else if (tokens[l].type == TK_DEC) {
            return atoi(tokens[l].str);
        } else if (tokens[l].type == TK_REG) {
            return isa_reg_str2val(tokens[l].str, success);
        } else {
            *success = false;
        }
    } else if (check_parentheses(l, r, success)) {
        return eval(l + 1, r - 1, success);
    } else {
        int bracket_cnt = 0;
        int result = -1;
        for (int i = r; i >= l; i--) {
            if (tokens[i].type == '(') {
                bracket_cnt--;
            } else if (tokens[i].type == ')') {
                bracket_cnt++;
            } else if (tokens[i].type=='+'||tokens[i].type=='-'){
                result=i;
                break;
            }else if(tokens[i].type=='*'||tokens[i].type=='/'||)
        }
        if (result == -1) {
            *success = false;
            return 0;
        }
    }
    return 0;

}


word_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    *success = true;
    /* TODO: Insert codes to evaluate the expression. */
    TODO();

    return 0;
}

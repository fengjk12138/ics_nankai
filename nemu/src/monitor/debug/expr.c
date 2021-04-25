#include <isa.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/vaddr.h>

enum {
    TK_NOTYPE = 256,
    TK_EQ, TK_NEQ,
    TK_AND, TK_OR, TK_NOT,
    TK_BQ, TK_SQ, TK_S, TK_B,
    TK_REG, TK_HEX, TK_DEC,
    TK_ACCESS

    /* TODO: Add more token types */

};

word_t vaddr_read(paddr_t, int);

static int priori[6][4] = {
        {TK_OR,  TK_AND},
        {TK_EQ,  TK_NEQ},
        {TK_B,TK_SQ,TK_S,TK_BQ},
        {'+',    '-'},
        {'*',    '/'},
        {TK_NOT, TK_ACCESS},
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

        {"0x[0-9a-f]*",    TK_HEX},
        {"\\(",         '('},
        {"\\)",         ')'},
        {"[1-9][0-9]*", TK_DEC},
        {"&&",          TK_AND},
        {"!=",          TK_NEQ},
        {"\\|\\|",      TK_OR},
        {"!",           TK_NOT},
        {"\\$[a-z]+",   TK_REG},
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


                tokens[nr_token].type = rules[i].token_type;
                for (int udp = 0; udp < substr_len; udp++)
                    tokens[nr_token].str[udp] = e[udp + position];
                tokens[nr_token].str[substr_len] = '\0';
                nr_token++;

                position += substr_len;

                /* TODO: Now a new token is recognized with rules[i]. Add codes
                 * to record the token in the array `tokens'. For certain types
                 * of tokens, some extra actions should be performed.
                 */


                switch (rules[i].token_type) {
                    case TK_NOTYPE:
                        nr_token--;
                        break;
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
    int bracket = 0;
    for (int i = l; i <= r; i++) {
        if (tokens[i].type == '(')
            bracket++;
        else if (tokens[i].type == ')')
            bracket--;
        else if (bracket == 0) {
            return false;
        }
    }
    if (tokens[l].type == '(' && tokens[r].type == ')') {

        return 1;
    } else if (tokens[l].type == '(') {
        printf("Brackets should match. Miss right bracket.");
        *success = false;
    } else if (tokens[r].type == ')') {
        printf("Brackets should match. Miss left bracket.");
        *success = false;
    }
    return 0;
}

int eval(int l, int r, bool *success) {
    if (l > r) {
        printf("please check your expr.");
        *success = false;
    } else if (l == r) {
        if (tokens[l].type == TK_HEX) {
            int ret = 0, len = strlen(tokens[l].str);
            for (int udp = 2; udp < len; udp++){
                ret = ret * 16 + (isdigit(tokens[l].str[udp])? tokens[l].str[udp]- '0':tokens[l].str[udp]-'a'+10);
            }
            return ret;
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
        int result = -1;
        for (int opt = 0; opt < 6; opt++) {
            int bracket_cnt = 0;
            for (int i = r; i >= l; i--) {
                if (tokens[i].type == '(') {
                    bracket_cnt--;
                } else if (tokens[i].type == ')') {
                    bracket_cnt++;
                } else if (bracket_cnt == 0) {
                    for (int j = 0; j < 4; j++)
                        if (priori[opt][j] != 0 && tokens[i].type == priori[opt][j]) {
                            result = i;
                            break;
                        }
                }
                if (result != -1) break;
            }
            if (result != -1) break;
        }
        if (result == -1) {
            *success = false;
            return 0;
        }
        switch (tokens[result].type) {
            case TK_NEQ:
                return eval(l, result - 1, success) != eval(result + 1, r, success);
                break;
            case TK_EQ:
                return eval(l, result - 1, success) == eval(result + 1, r, success);
                break;
            case TK_B:
                return eval(l, result - 1, success) > eval(result + 1, r, success);
                break;
            case TK_S:
                return eval(l, result - 1, success) < eval(result + 1, r, success);
                break;
            case TK_BQ:
                return eval(l, result - 1, success) >= eval(result + 1, r, success);
                break;
            case TK_SQ:
                return eval(l, result - 1, success) <= eval(result + 1, r, success);
                break;
            case TK_AND:
                return eval(l, result - 1, success) && eval(result + 1, r, success);
                break;
            case TK_OR:
                return eval(l, result - 1, success) || eval(result + 1, r, success);
                break;
            case TK_NOT:
                if (result != l) {
                    *success = false;
                    return 0;
                }
                return !eval(result + 1, r, success);
                break;
            case '+':
                return eval(l, result - 1, success) + eval(result + 1, r, success);
                break;
            case '*':
                return eval(l, result - 1, success) * eval(result + 1, r, success);
                break;
            case '/':
                return eval(l, result - 1, success) / eval(result + 1, r, success);
                break;
            case '-':
                return eval(l, result - 1, success) - eval(result + 1, r, success);
                break;
            case TK_ACCESS:
                if (result != l) {
                    *success = false;
                    return 0;
                }
                return (int) vaddr_read(eval(result + 1, r, success), 4);
                break;
            default:
                printf("can not recognize this operator: %s", tokens[result].str);
                *success = false;
                return 0;
        }
    }
    return 0;
}

bool check_is_access(int no) {
    if (tokens[no].type != '*')
        return false;
    if (no == 0)
        return true;
    if (tokens[no - 1].type == '+' || tokens[no - 1].type == '-' || tokens[no - 1].type == '*' ||
        tokens[no - 1].type == '/' || tokens[no - 1].type == '(' ||
        tokens[no - 1].type == TK_EQ || tokens[no - 1].type == TK_NEQ ||
        tokens[no - 1].type == TK_B || tokens[no - 1].type == TK_BQ || tokens[no - 1].type == TK_S ||
        tokens[no - 1].type == TK_SQ ||
        tokens[no - 1].type == TK_AND || tokens[no - 1].type == TK_OR || tokens[no - 1].type == TK_NOT)
        return true;
    return false;
}

word_t expr(char *e, bool *success) {

    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    *success = true;
    for (int i = 0; i < nr_token; i++) {
        if (check_is_access(i)) {
            tokens[i].type = TK_ACCESS;
        }
    }
    /* TODO: Insert codes to evaluate the expression. */
    return eval(0, nr_token - 1, success);
}

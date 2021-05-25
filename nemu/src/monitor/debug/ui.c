#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/vaddr.h>

void cpu_exec(uint64_t);

word_t vaddr_read(paddr_t, int);

int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char *rl_gets() {
    static char *line_read = NULL;

    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}

static int cmd_c(char *args) {
    cpu_exec(-1);
    return -1;
}


static int cmd_q(char *args) {
    return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args) {
    char *arg = strtok(NULL, " ");
    int run_num = 0;
    if (arg == NULL)
        run_num = 1;
    else run_num = atoi(arg);
    if (run_num <= 0)
        printf("args is not a number\n");
    cpu_exec(run_num);
    return 0;
}

static int cmd_info(char *args) {
    char *arg = strtok(NULL, " ");

    if (strcmp(arg, "r") == 0) {
        isa_reg_display();
    } else if (strcmp(arg, "w") == 0) {
        show_watchpoint();
    } else {
        printf("args should be \'r\' or \'w\'\n");
        return 0;
    }
    return 0;
}

static int cmd_p(char *args) {
    if (args == NULL) {
        printf("string length not enough\n");
        return 0;
    }
    bool succ = true;
    unsigned int result = expr(args, &succ);
    if (succ == false) {
        printf("please check your expr\n");
        return 0;
    }
    printf("0x%x\n", result);
    return 0;
}

static int cmd_x(char *args) {
//    printf("%s\n", args);
    char tmp[256];
    strcpy(tmp, args);
    char *arg = strtok(NULL, " ");

//    printf("%s\n", tmp + strlen(arg) + 1);
    bool succ = true;
    int padder = expr(tmp + strlen(arg) + 1, &succ);
    if (!succ) {
        printf("please check your scanf mem expr\n");
        return 0;
    }
    int read_num = atoi(arg);

    for (int i = 0; i < read_num; i++)
        printf("Mem = 0x%x, Val = 0x%x\n", padder + i * 4, (int) vaddr_read(padder + i * 4, 4));
    return 0;
}

static int cmd_w(char *args) {
    if (args == NULL) {
        printf("watchpoint string length not enough\n");
        return 0;
    }
    WP *watp = new_wp();
    bool succ = true;
    watp->val_before = expr(args, &succ);
    if (!succ) {
        free_wp(watp->NO);
        printf("please check your watchpoint expr\n");
        return 0;
    }
    strcpy(watp->expr, args);
    printf("watchponit add successful, now it\'s val is %d\n", watp->val_before);
    return 0;
}

static int cmd_d(char *args) {
    if (args == NULL) {
        printf("please input which watchpoint you watch to delete\n");
        return 0;
    }
    char *arg = strtok(NULL, " ");
    int run_num = atoi(arg);
    if (run_num == 0 && arg[0] != '0' && strlen(arg) != 1) {
        printf("please input correct number\n");
        return 0;
    }
    free_wp(run_num);
    printf("%d has been removed\n", run_num);
    return 0;
}

static struct {
    char *name;
    char *description;

    int (*handler)(char *);
} cmd_table[] = {
        {"help", "Display informations about all supported commands\n",  cmd_help},
        {"c",    "Continue the execution of the program\n",              cmd_c},
        {"q",    "Exit NEMU\n",                                          cmd_q},
        {"si",   "run one/any step:\"si n\"mean run n step\n",           cmd_si},
        {"info", "print register/watchpoint information:\"info r/w\"\n", cmd_info},
        {"p",    "print expr\'s val \n",                                 cmd_p},
        {"x",    "scan memory\'s val:\"x N epxr\"\n",                    cmd_x},
        {"w",    "add a watchpoint in memory\n",                         cmd_w},
        {"d",    "delete a watchpoint\n",                                cmd_d}
        /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    } else {
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

void ui_mainloop() {
//    if (is_batch_mode()) {
//        cmd_c(NULL);
//        return;
//    }

    for (char *str; (str = rl_gets()) != NULL;) {
        char *str_end = str + strlen(str);

        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if (cmd == NULL) { continue; }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end) {
            args = NULL;
        }

#ifdef HAS_IOE
        extern void sdl_clear_event_queue();
        sdl_clear_event_queue();
#endif

        int i;
        for (i = 0; i < NR_CMD; i++) {
            if (strcmp(cmd, cmd_table[i].name) == 0) {
                if (cmd_table[i].handler(args) < 0) { return; }
                break;
            }
        }

        if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
    }
}

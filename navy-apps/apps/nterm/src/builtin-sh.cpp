#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
    static char buf[256] = {};
    va_list ap;
    va_start(ap, format);
    int len = vsnprintf(buf, 256, format, ap);
    va_end(ap);
    term->write(buf, len);
}

static void sh_banner() {
    sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
    sh_printf("sh> ");
}

bool is_whitechar(char x) {
    return x == '\t' || x == ' ' || x == '\n' || x == '\f' || x == '\0';
}

static void sh_handle_cmd(const char *cmd) {
    setenv("PATH", "/bin", 0);
    char *empty[128];
    int len = strlen(cmd);
    char *new_cmd = (char *) malloc(len * sizeof(char));
    int now = 0;
    for (int i = 0; i < len; i++, now++)
        if (is_whitechar(cmd[i])) {
            new_cmd[i] = '\0';
            break;
        } else new_cmd[i] = cmd[i];
    while (is_whitechar(cmd[now]) && now < len)
        now++;
    int argc = 0;
    while (now < len) {
        if (!is_whitechar(cmd[now])) {
            argc++;
            empty[argc - 1] = (char *) malloc(32 * sizeof(char));
            int ar_len = 0;
            for (; !is_whitechar(cmd[now]); now++)
                empty[argc - 1][ar_len++] = cmd[now];
            empty[argc - 1][ar_len] = '\0';
        }
        now++;
    }
    empty[argc] = NULL;

    execvp(new_cmd, empty);
}

void builtin_sh_run() {
    sh_banner();
    sh_prompt();

    while (1) {
        SDL_Event ev;
        if (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
                const char *res = term->keypress(handle_key(&ev));
                if (res) {
                    sh_handle_cmd(res);
                    sh_prompt();
                }
            }
        }
        refresh_terminal();
    }
}

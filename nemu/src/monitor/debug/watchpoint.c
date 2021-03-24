#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
    int i;
    for (i = 0; i < NR_WP; i++) {
        wp_pool[i].NO = i;
        wp_pool[i].useful = false;
        wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].next = NULL;

    head = NULL;
    free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


void free_wp(int NO) {
    WP *free = NULL;
    if (head->NO == NO) {
        free = head;
        head = free->next;
    } else {
        free = head;
        while (1) {
            if (free->next->NO == NO) {
                WP *tmp = free->next;
                free->next = tmp->next;
                free = tmp;
                break;
            } else free = free->next;
        }
    }
    free->useful = false;
    free->next = free_;
    free_ = free;
}

WP *new_wp() {
    if (free_ == NULL) {
        printf("you add too much watchpoint\n");
        return NULL;
    }
    WP *ret = free_;
    free_ = free_->next;
    ret->useful = true;
    ret->next = head;
    head = ret;
    return ret;
}

int check_watchpoint() {
    WP *font = head;
    while (font != NULL) {
        bool succ = true;
        int val = expr(font->expr, &succ);
        if (!succ)
            return font->NO;
        if (val != font->val_before) {
            return font->NO;
        }
        font=font->next;
    }
    return -1;
}
void show_watchpoint(){
    WP *font = head;
    while (font != NULL) {
        printf("wp:%d, %s, val = %d\n",font->NO,font->expr,font->val_before);
        font=font->next;
    }
}
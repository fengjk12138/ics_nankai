#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
    int NO;
    bool useful;
    struct watchpoint *next;
    word_t val_before;
    char expr[256];
    /* TODO: Add more members if necessary */
} WP;

WP *new_wp();

void free_wp(int);

#endif

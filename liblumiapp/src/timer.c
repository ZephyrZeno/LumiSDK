/**
 * timer.c — Timer management
 * Copyright 2026 Lumi Team. Apache-2.0
 */

#include "lumiapp.h"
#include <stdlib.h>

#define MAX_TIMERS 256

typedef struct {
    int            id;
    uint32_t       delay_ms;
    bool           repeat;
    bool           active;
    lumi_timer_cb  callback;
    void          *userdata;
} timer_entry_t;

static timer_entry_t g_timers[MAX_TIMERS];
static int g_next_timer_id = 1;

int lumi_timer_set(uint32_t delay_ms, bool repeat, lumi_timer_cb cb, void *userdata) {
    if (!cb) return -1;

    for (int i = 0; i < MAX_TIMERS; i++) {
        if (!g_timers[i].active) {
            g_timers[i].id       = g_next_timer_id++;
            g_timers[i].delay_ms = delay_ms;
            g_timers[i].repeat   = repeat;
            g_timers[i].active   = true;
            g_timers[i].callback = cb;
            g_timers[i].userdata = userdata;

            lumi_log(LUMI_LOG_DEBUG, "timer", "Set timer %d: %ums %s",
                     g_timers[i].id, delay_ms, repeat ? "(repeat)" : "(once)");
            return g_timers[i].id;
        }
    }
    return -1; /* no free slots */
}

void lumi_timer_cancel(int timer_id) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (g_timers[i].active && g_timers[i].id == timer_id) {
            g_timers[i].active = false;
            lumi_log(LUMI_LOG_DEBUG, "timer", "Cancelled timer %d", timer_id);
            return;
        }
    }
}

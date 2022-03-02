#pragma once

#include <SDL.h>

typedef struct bmb_timer
{
    uint64_t current_tick;
    uint64_t prev_tick;
    float duration;
    double counter;
    

}bmb_timer_t;



void bmb_timer_tick(bmb_timer_t *timer);

int bmb_timer_stop(bmb_timer_t *timer);

void bmb_timer_reset(bmb_timer_t *timer);

void bmb_timer_update(bmb_timer_t *timer);

void bmb_timer_start(bmb_timer_t *timer, float duration);



#include <SDL.h>
#include "bmb_timer.h"

void bmb_timer_tick(bmb_timer_t *timer)
{

    timer->current_tick = SDL_GetPerformanceCounter();
    timer->counter -= (double)(timer->current_tick - timer->prev_tick) / SDL_GetPerformanceFrequency();
}

int bmb_timer_stop(bmb_timer_t *timer)
{

    int result = -1;
    if (timer->counter <= 0)
    {
        result = 0;
        bmb_timer_reset(timer);
    }

    return result;
}

void bmb_timer_reset(bmb_timer_t *timer)
{
    timer->counter = timer->duration;
}

void bmb_timer_update(bmb_timer_t *timer)
{
    timer->prev_tick=timer->current_tick;
}

void bmb_timer_start(bmb_timer_t *timer, uint64_t duration)
{

    timer->duration = duration;
    timer->counter = timer->duration;
    timer->prev_tick = SDL_GetPerformanceCounter();
}
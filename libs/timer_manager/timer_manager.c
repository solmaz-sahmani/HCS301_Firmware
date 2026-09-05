#include <stddef.h>

#include "timer_manager.h"

#define TIMER_MANAGER_MAX_TIMERS 8U

typedef struct
{
    bool active;
    uint32_t elapsed_ms;
    uint32_t duration_ms;

} timer_slot_t;

struct timer_manager
{
    timer_slot_t timers[TIMER_MANAGER_MAX_TIMERS];
};

static timer_manager_t timer_manager_instance;

timer_manager_t *timer_manager_get_instance(void)
{
    return &timer_manager_instance;
}

status_t timer_manager_init(timer_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < TIMER_MANAGER_MAX_TIMERS; i++)
    {
        manager->timers[i].active = false;
        manager->timers[i].elapsed_ms = 0U;
        manager->timers[i].duration_ms = 0U;
    }

    return STATUS_OK;
}

status_t timer_manager_start(
    timer_manager_t *manager,
    uint32_t duration_ms,
    timer_id_t *timer_id)
{
    if (manager == NULL || timer_id == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (duration_ms == 0U)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < TIMER_MANAGER_MAX_TIMERS; i++)
    {
        if (!manager->timers[i].active)
        {
            manager->timers[i].active = true;
            manager->timers[i].elapsed_ms = 0U;
            manager->timers[i].duration_ms = duration_ms;

            *timer_id = i + 1U;

            return STATUS_OK;
        }
    }

    return STATUS_FULL;
}

status_t timer_manager_is_expired(
    timer_manager_t *manager,
    timer_id_t timer_id,
    bool *expired)
{
    if (manager == NULL || expired == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (timer_id == TIMER_INVALID_ID ||
        timer_id > TIMER_MANAGER_MAX_TIMERS)
    {
        return STATUS_INVALID_ARG;
    }

    timer_slot_t *timer =
        &manager->timers[timer_id - 1U];

    if (!timer->active)
    {
        return STATUS_NOT_FOUND;
    }

    *expired =
        timer->elapsed_ms >= timer->duration_ms;

    return STATUS_OK;
}

status_t timer_manager_stop(
    timer_manager_t *manager,
    timer_id_t timer_id)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (timer_id == TIMER_INVALID_ID ||
        timer_id > TIMER_MANAGER_MAX_TIMERS)
    {
        return STATUS_INVALID_ARG;
    }

    timer_slot_t *timer =
        &manager->timers[timer_id - 1U];

    if (!timer->active)
    {
        return STATUS_NOT_FOUND;
    }

    timer->active = false;
    timer->elapsed_ms = 0U;
    timer->duration_ms = 0U;

    return STATUS_OK;
}

status_t timer_manager_update(
    timer_manager_t *manager,
    uint32_t elapsed_ms)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    for (uint32_t i = 0U; i < TIMER_MANAGER_MAX_TIMERS; i++)
    {
        timer_slot_t *timer = &manager->timers[i];

        if (!timer->active)
        {
            continue;
        }

        if (timer->elapsed_ms < timer->duration_ms)
        {
            uint32_t remaining =
                timer->duration_ms -
                timer->elapsed_ms;

            if (elapsed_ms >= remaining)
            {
                timer->elapsed_ms =
                    timer->duration_ms;
            }
            else
            {
                timer->elapsed_ms += elapsed_ms;
            }
        }
    }

    return STATUS_OK;
}
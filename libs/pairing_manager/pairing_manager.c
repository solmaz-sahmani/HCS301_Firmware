#include <stddef.h>

#include "pairing_manager.h"
#include "led_indicator.h"
#include "timer_manager.h"

#define PAIRING_LED_DURATION_MS 5000U
#define PAIRING_BLINK_INTERVAL_MS 500U

struct pairing_manager
{
    pairing_state_t state;

    timer_id_t success_timer;
    timer_id_t blink_timer;

    bool blink_state;
};

static pairing_manager_t pairing_manager_instance;

pairing_manager_t *pairing_manager_get_instance(void)
{
    return &pairing_manager_instance;
}

status_t pairing_manager_init(pairing_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    manager->state = PAIRING_IDLE;
    manager->success_timer = TIMER_INVALID_ID;
    manager->blink_timer = TIMER_INVALID_ID;
    manager->blink_state = false;

    return STATUS_OK;
}

status_t pairing_manager_start(pairing_manager_t *manager)
{
    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (manager->state != PAIRING_IDLE)
    {
        return STATUS_ERROR;
    }

    manager->state = PAIRING_WAITING;

    return STATUS_OK;
}

status_t pairing_manager_process_remote(
    pairing_manager_t *manager,
    const remote_info_t *remote)
{
    status_t status;

    if (manager == NULL || remote == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (manager->state != PAIRING_WAITING)
    {
        return STATUS_ERROR;
    }

    status = remote_storage_add(
        remote_storage_get_instance(),
        remote->serial,
        remote->counter);

    if (status != STATUS_OK)
    {
        return status;
    }

    manager->state = PAIRING_SUCCESS;

    return STATUS_OK;
}

status_t pairing_manager_process(
    pairing_manager_t *manager)
{
    timer_manager_t *timers;
    led_indicator_t *indicator;
    bool expired;

    if (manager == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    if (manager->state != PAIRING_SUCCESS)
    {
        return STATUS_OK;
    }

    timers = timer_manager_get_instance();
    indicator = led_indicator_get_instance();

    if (manager->success_timer == TIMER_INVALID_ID)
    {
        timer_manager_start(
            timers,
            PAIRING_LED_DURATION_MS,
            &manager->success_timer);

        timer_manager_start(
            timers,
            PAIRING_BLINK_INTERVAL_MS,
            &manager->blink_timer);

        manager->blink_state = true;

        led_indicator_all_on(indicator);

        return STATUS_OK;
    }

    timer_manager_is_expired(
        timers,
        manager->success_timer,
        &expired);

    if (expired)
    {
        led_indicator_all_off(indicator);

        timer_manager_stop(
            timers,
            manager->success_timer);

        timer_manager_stop(
            timers,
            manager->blink_timer);

        manager->success_timer = TIMER_INVALID_ID;
        manager->blink_timer = TIMER_INVALID_ID;
        manager->blink_state = false;
        manager->state = PAIRING_IDLE;

        return STATUS_OK;
    }

    timer_manager_is_expired(
        timers,
        manager->blink_timer,
        &expired);

    if (expired)
    {
        manager->blink_state =
            !manager->blink_state;

        if (manager->blink_state)
        {
            led_indicator_all_on(indicator);
        }
        else
        {
            led_indicator_all_off(indicator);
        }

        timer_manager_stop(
            timers,
            manager->blink_timer);

        timer_manager_start(
            timers,
            PAIRING_BLINK_INTERVAL_MS,
            &manager->blink_timer);
    }

    return STATUS_OK;
}

status_t pairing_manager_get_state(
    pairing_manager_t *manager,
    pairing_state_t *state)
{
    if (manager == NULL || state == NULL)
    {
        return STATUS_INVALID_ARG;
    }

    *state = manager->state;

    return STATUS_OK;
}
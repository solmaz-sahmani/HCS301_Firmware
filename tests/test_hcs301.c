#include <assert.h>
#include <stdio.h>

#include "hcs301.h"
#include "mock_hal.h"

static void test_button_pressed(void)
{
    mock_hal_set_gpio(0, true);

    bool result = hcs301_read_button(0);

    assert(result == true);
}

static void test_button_released(void)
{
    mock_hal_set_gpio(0, false);

    bool result = hcs301_read_button(0);

    assert(result == false);
}

int main(void)
{
    printf("Running HCS301 unit tests...\n");

    test_button_pressed();
    printf("test_button_pressed: PASS\n");

    test_button_released();
    printf("test_button_released: PASS\n");

    printf("All tests passed!\n");

    return 0;
}
#include <stdio.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "button_manager.h"
#include "button_input.h"

#include "rf_driver.h"
#include "rf_esp32.h"

#include "hcs301_protocol.h"
#include "hcs301_send.h"


#define RF_GPIO GPIO_NUM_19


void app_main(void)
{
    status_t status;

    printf("\n");
    printf("HCS301 ESP32 Transmitter\n");
    printf("========================\n");


    /*
     * --------------------------------------------------
     * Button Manager
     * --------------------------------------------------
     */

    button_manager_t *button_manager =
        button_manager_get_instance();

    status =
        button_manager_init(button_manager);

    if (status != STATUS_OK)
    {
        printf(
            "Button manager init failed: %d\n",
            status);

        return;
    }


    /*
     * --------------------------------------------------
     * Physical Button GPIO
     * --------------------------------------------------
     */

    status =
        button_input_init();

    if (status != STATUS_OK)
    {
        printf(
            "Button input init failed: %d\n",
            status);

        return;
    }


    /*
     * --------------------------------------------------
     * RF Driver
     * --------------------------------------------------
     */

    rf_driver_t *rf =
        rf_driver_get_instance();


    /*
     * Initialize ESP32 RF backend.
     *
     * FS1000A DATA -> GPIO17
     */

    rf_hal_t rf_hal = {0};

    status =
        rf_esp32_init(
            &rf_hal,
            RF_GPIO);

    if (status != STATUS_OK)
    {
        printf(
            "RF ESP32 init failed: %d\n",
            status);

        return;
    }


    /*
     * Initialize RF driver.
     */

    status =
        rf_driver_init(
            rf,
            &rf_hal);

    if (status != STATUS_OK)
    {
        printf(
            "RF driver init failed: %d\n",
            status);

        return;
    }


    /*
     * --------------------------------------------------
     * HCS301 Protocol
     * --------------------------------------------------
     */

    hcs301_protocol_t *protocol =
        hcs301_protocol_get_instance();

    status =
        hcs301_protocol_init(protocol);

    if (status != STATUS_OK)
    {
        printf(
            "HCS301 protocol init failed: %d\n",
            status);

        return;
    }


    /*
     * --------------------------------------------------
     * HCS301 Sender
     * --------------------------------------------------
     */

    hcs301_send_t *sender =
        hcs301_send_get_instance();

    status =
        hcs301_send_init(
            sender,
            protocol,
            rf);

    if (status != STATUS_OK)
    {
        printf(
            "HCS301 sender init failed: %d\n",
            status);

        return;
    }


    /*
     * --------------------------------------------------
     * Previous button states
     *
     * Used for detecting:
     *
     * released -> pressed
     * --------------------------------------------------
     */

    bool previous_state[BUTTON_COUNT] = {false};


    printf("\n");
    printf("Transmitter ready\n");
    printf("A -> GPIO4 -> BUTTON_1\n");
    printf("B -> GPIO5 -> BUTTON_2\n");
    printf("C -> GPIO6 -> BUTTON_3\n");
    printf("D -> GPIO7 -> BUTTON_4\n");
    printf("RF -> GPIO17\n");
    printf("\n");


    /*
     * --------------------------------------------------
     * Main application loop
     * --------------------------------------------------
     */

    while (true)
    {
        /*
         * Read physical GPIO buttons
         * and update button_manager.
         */

        status =
            button_input_update();

        if (status != STATUS_OK)
        {
            printf(
                "Button input update failed: %d\n",
                status);

            vTaskDelay(
                pdMS_TO_TICKS(10));

            continue;
        }


        /*
         * Check buttons A/B/C/D.
         */

        for (button_id_t button = BUTTON_1;
             button <= BUTTON_4;
             button++)
        {
            bool pressed = false;


            /*
             * Get current button state
             * from button_manager.
             */

            status =
                button_manager_is_pressed(
                    button_manager,
                    button,
                    &pressed);

            if (status != STATUS_OK)
            {
                continue;
            }


            /*
             * Detect button press edge.
             *
             * Released -> Pressed
             *
             * This prevents continuous transmission
             * while the button remains pressed.
             */

            if (pressed &&
                !previous_state[button])
            {
                printf(
                    "Button %d pressed\n",
                    (int)(button - BUTTON_1 + 1));


                /*
                 * Send HCS301 frame.
                 *
                 * hcs301_send is responsible for:
                 *
                 * button_status
                 *       ↓
                 * HCS301 frame
                 *       ↓
                 * KeeLoq encryption
                 *       ↓
                 * 66-bit frame
                 *       ↓
                 * RF pulse encoding
                 *       ↓
                 * RF driver
                 */

                status =
                    hcs301_send_button(
                        sender,
                        button);

                if (status != STATUS_OK)
                {
                    printf(
                        "HCS301 transmit failed: %d\n",
                        status);
                }
                else
                {
                    printf(
                        "Button %d transmitted\n",
                        (int)(button - BUTTON_1 + 1));
                }
            }


            /*
             * Save current state for
             * next iteration.
             */

            previous_state[button] = pressed;
        }


        /*
         * Polling interval.
         *
         * 10 ms
         */

        vTaskDelay(
            pdMS_TO_TICKS(10));
    }
}
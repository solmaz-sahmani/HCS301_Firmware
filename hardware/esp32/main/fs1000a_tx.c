#include "fs1000a_tx.h"

#include "driver/gpio.h"
#include "esp_timer.h"


#define FS1000A_TX_GPIO 17


#define RF_DELAY_US 400


static void rf_delay(void)
{
    esp_rom_delay_us(RF_DELAY_US);
}



static void send_bit(
        uint8_t bit)
{

    if(bit)
    {
        gpio_set_level(
                FS1000A_TX_GPIO,
                1);

        rf_delay();


        gpio_set_level(
                FS1000A_TX_GPIO,
                0);

        rf_delay();

    }
    else
    {

        gpio_set_level(
                FS1000A_TX_GPIO,
                1);

        rf_delay();


        gpio_set_level(
                FS1000A_TX_GPIO,
                0);

        rf_delay();

    }

}



esp_err_t fs1000a_tx_init(void)
{

    gpio_config_t io =
    {
        .pin_bit_mask =
            (1ULL << FS1000A_TX_GPIO),

        .mode =
            GPIO_MODE_OUTPUT,

        .pull_up_en =
            GPIO_PULLUP_DISABLE,

        .pull_down_en =
            GPIO_PULLDOWN_DISABLE,

        .intr_type =
            GPIO_INTR_DISABLE
    };


    return gpio_config(&io);

}




esp_err_t fs1000a_tx_send(
        const uint8_t *bits,
        uint16_t length)
{

    if(bits == NULL)
        return ESP_ERR_INVALID_ARG;



    for(uint16_t i=0;i<length;i++)
    {
        send_bit(bits[i]);
    }


    gpio_set_level(
            FS1000A_TX_GPIO,
            0);


    return ESP_OK;
}
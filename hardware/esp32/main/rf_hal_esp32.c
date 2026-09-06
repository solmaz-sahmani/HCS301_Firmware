#include "rf_hal_esp32.h"

#include "fs1000a_tx.h"



esp_err_t rf_hal_init(void)
{
    return fs1000a_tx_init();
}



esp_err_t rf_hal_send(
        const uint8_t *data,
        uint16_t length)
{

    return fs1000a_tx_send(
            data,
            length);

}
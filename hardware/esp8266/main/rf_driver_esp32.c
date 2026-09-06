#include "rf_driver_esp32.h"
#include "hcs301_rf_tx.h"

esp_err_t rf_driver_esp32_init(void)
{
    return hcs301_rf_tx_init();
}

esp_err_t rf_driver_esp32_transmit_frame(const uint8_t bits[66])
{
    return hcs301_rf_tx_send(bits);
}

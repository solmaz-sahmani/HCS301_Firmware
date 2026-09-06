#include "rf_driver_stm32.h"

#include "tim.h"

static hcs301_rf_rx_t rf_receiver;

HAL_StatusTypeDef rf_driver_stm32_init(void)
{
    HAL_StatusTypeDef status;

    status = hcs301_rf_rx_init(&rf_receiver);

    if (status != HAL_OK)
    {
        return status;
    }

    return hcs301_rf_rx_start(&rf_receiver, &htim3);
}

bool rf_driver_stm32_receive(uint8_t bits[66])
{
    return hcs301_rf_rx_read(&rf_receiver, bits);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        hcs301_rf_rx_capture_edge(&rf_receiver, htim);
    }
}

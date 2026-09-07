#ifndef RF_DRIVER_H
#define RF_DRIVER_H

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    bool level;
    uint16_t duration_us;

} rf_pulse_t;



typedef struct rf_driver rf_driver_t;



status_t rf_driver_init(
        rf_driver_t *driver);



status_t rf_driver_transmit(
        rf_driver_t *driver,
        const rf_pulse_t *pulses,
        uint32_t count);


#endif
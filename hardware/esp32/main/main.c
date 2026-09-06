#include "rf_hal_esp32.h"


void app_main(void)
{

    uint8_t frame[66]={0};


    frame[0]=1;
    frame[10]=1;
    frame[20]=1;
    frame[50]=1;



    rf_hal_init();



    while(1)
    {

        rf_hal_send(
                frame,
                66);


    }

}
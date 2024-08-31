#include  <msp430.h>
#include  "api.h"
#include  "hal.h"
#include  "bsp.h"



//******************************************************************
// TODO
//******************************************************************

int main(void)
{

    // init_system
    sys_init();

    //init gpio
    GPIO_init();

    LCD_init();





    while(1){
        switch(state){ //selection- from PC
        case 8:
            lpm(1);
            break;
        case 1:
            Manual_control();
           // ADC10_OFF();
            break;
        case 2:
            enable_pb();
            Painter();
            disable_pb();
            break;
        case 3:
            Calibrate();
            disable_pb();
            break;
        case 4:
             lcd_clear();
             Script_mode();
            // state = 8;

            break;
        }
    }
}

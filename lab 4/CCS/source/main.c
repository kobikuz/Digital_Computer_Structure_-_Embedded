#include  <msp430.h>
#include  "api.h"
#include  "hal.h"
#include  "bsp.h"



//******************************************************************
// TODO
//******************************************************************
// todo- frequency counter (extra)
// todo- getX, interrupt RX TX
// todo- clear, print menu, sleep

int main(void)
{
    // init_system
    sys_init();

    //init gpio
    GPIO_init();

    //init lcd
    LCD_init();


    while(1){
        switch(selection){
        case 8:
            lcd_clear();
            lcd_puts("LAB 4");
            lpm(1);
            break;
        case 1:
            lcd_clear();
            lcd_puts("state 1");
            BlinkRGB(x);
            break;
        case 2:
            lcd_clear();
            lcd_puts("state 2");
            Counter(x);
            break;
        case 3:
            lcd_clear();
            lcd_puts("state 3");
            CircBuzzer(x);
            break;
        case 4:
            lcd_clear();
            lcd_puts("state 4");
            selection = 8;
            //GetX(500);
            break;
        case 5:
            cursor_off;
            lcd_clear();
            lcd_puts("state 5");
            LDR_VAL();
            break;
        case 6:
            lcd_clear();
            selection = 8;
            break;
        case 9:
            lcd_clear();
            lcd_puts("state 9");
            real_time();
            //selection = 8;
            break;
        }
    }
}

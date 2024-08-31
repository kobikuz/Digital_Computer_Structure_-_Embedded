
//#include  <msp430xG46x.h> /* include peripheral declarations */
//#include  "bsp_msp4.h"    // private library

#include <msp430.h>
#include  "bsp_msp2.h"    // private library
#include  "hal.h"    // private library
#include  "api.h"    // private library

void main(){

InitGPIO();
lcd_init();
lcd_clear();

    while(1) {
        switch(selector){
        case 0: //sleep
            enable_allpb();
            _BIS_SR(LPM3_bits + GIE);
            int x = 1;
            break;
        case 1:
            enable_allpb();
             TIMER1_A2_config();
            freqMeas();
            break;
        case 2:
            enable_allpb();
            lcd_clear();          // Clear LCD screen
            CountUP();          // Perform stop watch
            break;
        case 3:
            enable_allpb();
            ADCconfig();
            TIMER1_A1_config();
            tone_generator();
            break;
        case 4:
            enable_allpb();
            lcd_clear();          // Clear LCD screen
            real_time();
            break;
        }


    }
}

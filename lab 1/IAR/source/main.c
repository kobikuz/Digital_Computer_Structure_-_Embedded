
#include  <msp430xG46x.h> /* include peripheral declarations */
#include  "bsp_mspx4xx.h"    // private library

//#include <msp430.h>
//#include  "bsp_mspx2xx.h"    // private library


#include  "hal.h"    // private library
#include  "api.h"    // private library


void main(){

InitGPIO();

  while(1) { 
  switch(selector){
    case 0: //Sleep Mode
      clearLED();
      enable_allpb();
      _BIS_SR(LPM3_bits + GIE);
      int x = 1;
      break;     
    case 1: //state1- count

      selector = 0;
      enable_pb1();
      if (flag ==0){        
      countup(&curr_num);
      }
      if (flag ==1){
        countdown(&curr_num);
      }

      enable_allpb();

      break;
    case 2: //state2- running LED
      selector = 0;
      runningLED(&shift_num);
      clearLED();
      enable_allpb();    
      break;
    case 3: //state3- PWM 2.7
     // selector = 0;
      clearLED();
      enable_allpb();     
      PWM();
      break;
  }
 }
}

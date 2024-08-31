

#include  <msp430xG46x.h> /* include peripheral declarations */
#include  "bsp_mspx4xx.h"    // private library

//#include <msp430.h>
//#include  "bsp_mspx2xx.h"    // private library


#include  "hal.h"    // private library
#include "api.h"



void countup(char* curr_num){ 
   enable_pb1();
   int i;
  for ( i=0; i<20; i++){
    *curr_num = *curr_num + 1;  
    LEDsArrayShow(*curr_num);
     Timer05();
     if (flag ==1)   break;
}
selector = 0;
}
void countdown(char* curr_num){ 
   enable_pb1();
   int i;
  for ( i=0; i<20; i++){
    *curr_num= *curr_num - 1;  
    LEDsArrayShow(*curr_num);
     Timer05();
     if (flag ==0)  break;  
  }
}

void runningLED(char* shift_num){
  int i;
  for(i=0; i<14; i++){
    if (*shift_num == 0 ){
      *shift_num =1;
    }
    LEDsArrayShow(*shift_num);
   *shift_num = *shift_num<<1;
    Timer05();
  }
}
void clearLED(void){
    LEDsArrayClear(0xff);
}

void PWM(void){
  _BIS_SR(GIE);
  while(selector==3){
    int i;
    PWMoff();
    for(i=0;i<10;i++);
    PWMon();
    for(i=0;i<34;i++);
  }
  PWMoff();
}

void Timer05(void){
  volatile unsigned int i;
  for(i=52500; i>0; i--);
  for(i=52500; i>0; i--);
}


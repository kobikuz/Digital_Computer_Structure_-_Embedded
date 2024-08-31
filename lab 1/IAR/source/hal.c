

#include  <msp430xG46x.h> /* include peripheral declarations */
#include  "bsp_mspx4xx.h"    // private library

//#include <msp430.h>
//#include "bsp_mspx2xx.h"

#include  "hal.h"    // private library
#include  "api.h" 

int selector = 0;
int flag = 1;
char curr_num =0;
char shift_num =1;

// LEDs Operation Functions
void LEDsArrayToggle(char a){
  LEDsArrayPort ^= a;
}
//------------------------------------------------------------------
void LEDsArraySet(char a){
  LEDsArrayPort |= a;
}
//------------------------------------------------------------------
void LEDsArrayShow(char a){
  LEDsArrayPort = a;
}
//------------------------------------------------------------------
void LEDsArrayClear(char a){
  LEDsArrayPort &= ~a;
}
// PWM Operation Functions
//******************************************************************

void PWMon(){
  PWMPort |= 0x80;
}
//-----------------------------------------------------------------
void PWMoff(){
  PWMPort &= ~0x80;
}
// Port1 Interrupt Service Rotine
//******************************************************************
#pragma vector=PORT2_VECTOR
  __interrupt void PB_ISR (void){
    int i;
    if (PBsArrayIntPending & 0x01){
          selector = 1; //case of PB P2.0
          flag = flag^1;
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
          while(!PBsArrayIntPending & 0x01);// wait of release PB.0 button
		for(i=1000 ; i>0 ; i--); //delay, button debounce
          PBsArrayIntPending &= ~0x01; 
 
    }
        
    else if(PBsArrayIntPending & 0x02){
          selector = 2; //case of PB P2.1
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrayIntPending & 0x02);// wait of release PB.0 button
		for(i=1000 ; i>0 ; i--); //delay, button debounce
          PBsArrayIntPending &= ~0x02;    

    }
      
    else if(PBsArrayIntPending & 0x04){
          selector = 3; //case of PB P2.2
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrayIntPending & 0x04);// wait of release PB.0 button
		for(i=1000 ; i>0 ; i--); //delay, button debounce
          PBsArrayIntPending &= ~0x04;

    }   
    
    else if(PBsArrayIntPending & 0x08){
          selector = 4; //case of PB P2.3
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrayIntPending & 0x08);// wait of release PB.0 button
		for(i=1000 ; i>0 ; i--); //delay, button debounce
          PBsArrayIntPending &= ~0x08;

    }   
    _BIC_SR(GIE); 
    PBsArrayIntEnable &= ~0x0f;
    _BIC_SR_IRQ(LPM3_bits);    // exit LPM3 bits from 0(SR)

    
    //PBsArrayIntPending &= ~0x0f; // manual clear of p2.0-2 interrupt flags 
  }

  void enable_pb1(void){
    PBsArrayIntEnable |= 0x01;
      
  }
 void no_interpts(void){
 //  PBsArrayIntEnable &= ~0x0f;
    _BIC_SR(GIE);
  }
 void enable_allpb(void){
   PBsArrayIntEnable = 0x0f;
   _BIS_SR(GIE);
  }

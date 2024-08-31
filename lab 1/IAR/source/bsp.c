
#include  <msp430xG46x.h> /* include peripheral declarations */
#include  "bsp_mspx4xx.h"    // private library

//#include <msp430.h>
//#include  "bsp_mspx2xx.h"    // private library




void InitGPIO(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LEDs 8-bit Array Port configuration
  LEDsArrayPortDir = 0xff;
  LEDsArrayPort = 0x00;   			// clear LEDs port- WAS 0XFF
  //LEDsArrayPortSel = 0x00; 		// only for "MSP430G2xx3"- we changed here
  LEDsArrayPortSel &= ~0xFF;        // FROM TOMER
  
  // PBs 4-bit Array Port configuration  
  //PBsArrayPort    = 0x00;
  PBsArrayPortSel &=  ~0xff;             // PB2.all as GPIO
  PBsArrayPortDir &= ~0x0f;             // PB.0 - 3 as GPIO-input
  PBsArrayIntEdgeSel |=0x03;   // MAABADA SHIT
  PBsArrayIntEdgeSel &=~0x0C;
  PBsArrayIntEnable = 0x0f;            // enable PB.0 - 3 interrupts , was 0x0f
  PBsArrayIntPending = 0x00;            // clear all interrupts pending
  //PBsArrayIntEdgeSel |=0x0f;            // PB.0 - 3 as falling-down edge select



  
  //PWM init define
  PBsArrayPortDir |= 0x80;                        // P2.7 as GPIO-output
  PWMPort &= ~0x80;
  
}

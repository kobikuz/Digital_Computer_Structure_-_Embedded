#include "api.h"
#include "hal.h" // private library - API layer
#include "bsp.h"

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF;
  LCD_DATA_DIR |= 0xF0;    // DIR P1.4-P1.7 To Output('1')
  LCD_DATA_SEL &= ~0xF0;   // Sel Bit clear P1.4-P1.7
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7
  

  // Keypad Setup
  KeypadPortSel &= ~0xFF;
  KeypadPortDIR = 0x0F; //10.0-10.3 output, 10.4-10.7 input
  KeypadPortOUT = 0x00; // CLR output

  // Keypad IRQ Setup
  KeypadIRQPortSel &= ~BIT1;
  KeypadIRQPortDir &= ~BIT1;             // P2.1 input
  KeypadIRQIntEdgeSel |= BIT1;         // pull-up mode  P2.1 - '1'
  KeypadIRQIntEn |= BIT1;               // P2.1 - '1'
  KeypadIRQIntPend &= ~0xFF;            // clear pending interrupts P2.1

  // PushButtons Setup
  PBsArrPortSel &= ~0x07;           //
  PBsArrPortOut &= ~0x07;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x07;            // P1.0-2 - Input ('0')
  PBsArrIntEdgeSel |= 0x03;  	     // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x07;               // P1.0-2 - '1'
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1
  //LED Setup
  //LEDS

  LEDARRAYSEL = ~0xff;
  LEDARRAYDIR = 0xff;
  LEDARRAY    = 0x00;

  


//  // PushButton 3 Setup For Main Lab
//   PB3sArrPortSel &= ~BIT0;           //
//   PB3sArrPortOut &= ~BIT0;            // Set P2Out to '0'
//   PB3sArrPortDir &= ~BIT0;            // P2.0 - Input ('0')
//   PB3sArrIntEdgeSel &= ~BIT0;         // pull-down mode  P2.0 - '0'
//   PB3sArrIntEn |= BIT0;               // P1.0-2 - '1'
//   PB3sArrIntPend &= ~BIT0;            // clear pending interrupts P2.0

  _BIS_SR(GIE);                     // enable interrupts globally


}

//-------------------------------------------------------------------------------------
//            Timer A 345msec configuration
//-------------------------------------------------------------------------------------

void startTimerB(){
    TBCTL |= MC_1 + TBIE;
    TBCCTL2 |= CCIE;
    _BIS_SR(LPM1_bits + GIE);


    //TBCCTL0 |= CCIE;
}
//-------------------------------------------------------------------------------------
//            Timer B PWM configuration
//-------------------------------------------------------------------------------------
void TIMERB_config(void){
    TBCTL = TBSSEL_2 + MC_0+ID_3; //SMCLK + count mode
    TBCCR2 = 62500;
    TBCCR0 = 0xFFFF;

      //  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
      //  TBCCR2 = 524288;
     //   TBCCTL2 = 0;
     //   TBCCR2 = 0;
     //   TBCCTL0 = CCIE;
      //  TB0CTL = TASSEL_2 + MC_0 + ID_3;
}
//-------------------------------------------------------------------------------------
//            DMA configuration
//-------------------------------------------------------------------------------------
void DMA_config(void){
    //DMA0DA = (int) &TBCCR0;
    //DMA0CTL = DMAEN + DMASRCINCR_3 + DMASWDW + DMAIE; //repeated-single, source inc, word-word trans, Interupt enable
    DMACTL0 = DMA0TSEL_0; // transfer via REQ
    DMA0CTL = DMADT_1 | DMADSTINCR_3 | DMASRCINCR_3 | DMASBDB| DMAIE  ; // block, dont inc D&S,move w.
    DMA0CTL |= DMAEN;
   // DMACTL0 = DMA0TSEL_1; //TACCR2_IFG trigger
}

void DMA_config_Task3(void){ // For Main Lab
    DMACTL0 = DMA0TSEL_8;
    DMA0CTL = DMADT_1 | DMADSTINCR_0 | DMASRCINCR_0 | DMASBDB  ;
    //DMA0CTL |= DMAEN;
}
//-------------------------------------------------------------------------------------
//            Stop All Timers
//-------------------------------------------------------------------------------------
void StopAllTimers(void){

    TACTL = MC_0; // halt timer A
    TBCTL = MC_0; // halt timer B
    TBCCTL1 = 0x00; // stop PWM
    //DMA0CTL = 0; // Stop DMA0


}





//#include  <msp430xG46x.h> /* include peripheral declarations */
//#include  "bsp_msp4.h"    // private library

#include <msp430.h>
#include "bsp_msp2.h"

#include  "hal.h"
#include  "api.h"

int selector = 0;
unsigned int Count = 0x0;
unsigned int REdge1, REdge2;
int flag = 1;// start when stopped

// Port1 Interrupt Service Rotine
//******************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PB_ISR (void){
    int i;
    if (PBsArrayIntPending & 0x01){
          selector = 1; //case of PB P2.0
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

    _BIC_SR(GIE);
    PBsArrayIntEnable &= ~0x0f;
    _BIC_SR_IRQ(LPM3_bits);    // exit LPM3 bits from 0(SR)


    //PBsArrayIntPending &= ~0x0f; // manual clear of p2.0-2 interrupt flags
  }

#pragma vector=PORT2_VECTOR
  __interrupt void SW1_ISR (void){
      int i;
      if (SwitchIFG  & 0x01){
          if (SwitchIES & 0x01){ //we are in falling edge
             flag =1;
             SwitchIES &= ~0x01;
          }
          else{
              flag =0;
             SwitchIES |= 0x01; // make it falling edge
          }
          SwitchIFG &= ~0x01; //clearIFG
      }
      else if(STATE4IFG & 0x02){
            selector = 4;
            for(i=1000 ; i>0 ; i--);  //delay, button debounce
             while(!STATE4IFG & 0x02);// wait of release PB.0 button
          for(i=1000 ; i>0 ; i--); //delay, button debounce
          STATE4IFG &= ~0x02;

      }
      else
          SwitchIFG &= ~0xff;
      _BIC_SR_IRQ(LPM0_bits);
  }



void enable_allpb(void){
   PBsArrayIntEnable |= 0x07;
   STATE4IE |=0x02;
  _BIS_SR(GIE);
 }

void disable_interrupts(void){
  _BIC_SR(GIE);
}

//            TimerA1 Interrupt Service Routine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TIMER1_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) TIMER1_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(TA1IV, 0x0A))
  {
      case  TA1IV_NONE: break;              // Vector  0:  No interrupt
      case  TA1IV_TACCR1:                   // Vector  2:  TACCR1 CCIFG
          TA1CTL &= ~(TAIFG);
        break;
      case TA1IV_TACCR2:                    // Vector  4:  TACCR2 CCIFG
          if (TA1CCTL2 & CCI)                 // Capture Input Pin Status
                  {
                      // Rising Edge was captured
                      if (!Count)
                      {
                          //REdge1 = TA1CCR2;
                          REdge1 = TA1CCR2;
                          Count++;
                      }
                      else
                      {
                          REdge2 = TA1CCR2;
                          TA1CCTL2 &= ~CCIE;
                          Count=0x0;
                          __bic_SR_register_on_exit(LPM0_bits + GIE);  // Exit LPM0 on return to main
                      }


                  }
          break;
      case TA1IV_6: break;                  // Vector  6:  Reserved CCIFG
      case TA1IV_8: break;                  // Vector  8:  Reserved CCIFG
      case TA1IV_TAIFG: break;              // Vector 10:  TAIFG
      default:  break;
  }
}

//            TimerA0 Interrupt Service Routine
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{

    LPM0_EXIT;
    TACTL = MC_0+TACLR; //wake up and clear--Bob marley 1947
}

//freq temp to LCD
void write_freq_temp_LCD(void){
    lcd_clear();
    lcd_home();
     const char SquareWaveFreq[] = "fin=";
     const char Hz[] = "Hz";
      lcd_puts(SquareWaveFreq);
      lcd_cursor_right();
      lcd_cursor_right();
      lcd_cursor_right();
      lcd_cursor_right();
      lcd_cursor_right();
      lcd_puts(Hz);
 }
//    write signal shape template to LCD
//******************************************************************
void write_signal_shape_tmp_LCD(){
   lcd_clear();
   lcd_home();
    const char signal_shape[] = "signal shape: ";
     lcd_puts(signal_shape);
     lcd_new_line;
}

// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}

// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm(" nop");
  asm(" nop");
  LCD_EN(0);
}

//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    __bic_SR_register_on_exit(CPUOFF);
}

///realtime


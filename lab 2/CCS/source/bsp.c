
//#include  <msp430xG46x.h> /* include peripheral declarations */
//#include  "bsp_msp4.h"    // private library

#include <msp430.h>
#include  "bsp_msp2.h"    // private library
#include  "hal.h"    // private library
#include "api.h"

void InitGPIO(void){
 // volatile unsigned int i; // in case of while loop usage

  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT



  //SW init - to check!
  SwitchSEL &= ~0x01;
  SwitchDIR &= ~0x01;
  SwitchREN |= 0x01;
  SwitchOUT |= 0x01;
  SwitchIE  |= 0x01;
  SwitchIES &= ~0x01; ///rising edge select
  SwitchIFG &= ~0x01;

  // init generator
  SwitchSEL |= 0x10;
  SwitchDIR &= ~0x10;

  //config P2.2
  SwitchSEL |= 0x04;
  SwitchDIR |= 0x04;

  //P1.3 config
  PBsArrayPortSel |= 0x08 ; //for P1.3- NOT?>
   PBsArrayPortDir &= ~0x08 ;  //for P1.3

   // LCD configuration
     LCD_DATA_WRITE &= ~0xFF;
     LCD_DATA_DIR |= 0xF0;    // P1.4-P1.7 To Output('1')
     LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7
     LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7

     // PBs 4-bit Array Port configuration
     PBsArrayPortSel &=  ~0xff;             // PB2.all as GPIO
     PBsArrayPortDir &= ~0x0f;             // PB.0 - 3 as GPIO-input
     PBsArrayIntEdgeSel |=0x03;   // MAABADA SHIT
     PBsArrayIntEdgeSel &=~0x0C;
     PBsArrayIntEnable |= 0x07;            // enable PB.0 - 3 interrupts , was 0x0f

     PBsArrayIntPending = 0x00;            // clear all interrupts pending


     //REAL TIME
     STATE4SEL &=  ~0x02;             // PB2.all as GPIO
     STATE4DIR &= ~0x02;             // PB.0 - 3 as GPIO-input
     STATE4IES &=~0x02;   // MAABADA SHIT
     STATE4IE |= 0x02;            // enable PB.0 - 3 interrupts , was 0x0f
     STATE4IFG = 0x00;            // clear all interrupts pending
}
void TIMER0_A0_config(void){ //state 2 -goes for 1 sec
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    TA0CCTL0 = CCIE;
    TACCR0 = 0xFFFF;      //max value - notice that i use a diff clock for hadad
    TA0CTL = TASSEL_2 + MC_0 ;  //  select: 2 - aclk=2^15 ; control: 3 - Up/Down  ; divider: 3 - /8,
    //TA0CTL = TASSEL_1 + MC_0 ;
   // __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

void TIMER1_A2_config(void){ //STATE 1
      TA1CCTL2 = CAP + CM_1 + SCS + CCIS_0 + CCIE; // Timer1 configuration; // capture+rising edge+IE+synch.+
    // CM_1 - * Capture mode: 1 - pos. edge */
    // SCS - /* Capture synchronize */
}

//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
                LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
                LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
                LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

//******************************************************************
//state 3
//******************************************************************
void TIMER1_A1_config(void){ //state 3
    TA1CCTL1 =  OUTMOD_7; // TA1CCR1 reset/set;
}

void ADCconfig(void){ //state 3
    // Configure ADC10 control register 0
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + SREF_0 + ADC10IE;
    // ADC10SHT_2: Sample-and-hold time = 16 x ADC10CLKs
    // ADC10ON: ADC10 on
    // SREF_0: Vcc AND gnd
    // ADC10IE: ADC10 interrupt enabled

    // Configure ADC10 control register 1
    ADC10CTL1 = INCH_3 + ADC10SSEL_3;
    // INCH_3: Input channel 3 (P1.3)
    // ADC10SSEL_3: SMCLK as clock source

    // Enable analog input on P1.3
    ADC10AE0 |= BIT3;
    // BIT3: Enable ADC input on P1.3
}
//******************************************************************
//state 4
//******************************************************************

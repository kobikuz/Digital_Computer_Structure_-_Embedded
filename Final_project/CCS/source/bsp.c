#include  <msp430.h>
#include  "bsp.h"
#include <math.h>
/*
 * bsp.c
 *
 *  Created on: 11 במאי 2023
 *      Author: hagoz
 */

void GPIO_init(){




    //LDR
    LDRSEL |= BIT3;    //as ADC--????
    LDRDIR &= ~BIT3;     // as ADC-input

    //UART
    UARTSEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2 = TXD
    P1SEL2 = BIT1 + BIT2;
}

void sys_init(){

    WDTCTL = WDTHOLD | WDTPW;                            // stop watchdog timer


    TA0CTL = TASSEL_2 + MC_0 + ID_3;                     //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8

    //ADC
     ADC10CTL1 = INCH_3+ CONSEQ_3+ ADC10SSEL_3;                   //  SMCLK and series and mult. channels
     ADC10CTL0 = ADC10SHT_2+ ADC10ON  + ADC10IE+MSC +SREF_0;//16 * ADCLK + Turn on, set ref to Vcc and Gnd/

     ADC10AE0  |= 0x09;                                   //FOR X AND Y
     ADC10DTC1 = 0x10;                         // 16 conversions


     //UART

     DCOCTL = 0;                                         // Select lowest DCOx and MODx settings
     BCSCTL1 = CALBC1_1MHZ;                              // Set DCO
     DCOCTL = CALDCO_1MHZ;

     UCA0CTL1 |= UCSSEL_2;                               // UCA0 - SMCLK
     UCA0BR0 = 104;                                      //  UCA0 - Baud Rate 16MHz 9600
     UCA0BR1 = 0;
     UCA0MCTL = UCBRS0;                                  // Modulation UCBRSx = 1
     UCA0CTL1 &= ~UCSWRST;                               // **Initialize USCI state machine**
     IE2 = UCA0RXIE;                                     // UCA0 Receive interrupt enabled

     //Stepper
     Stepper_Phase_DIR |=0x0f;
     Stepper_Phase_SEL  &= ~0x0f;
     Stepper_Phase_OUT &= ~0x0f;

     // SW PB

     Joystick_PB_SEL &= ~0x10;  // Set P1.4 as GPIO
     Joystick_PB_DIR &= ~0x10;  // Set P1.4 as input
     Joystick_PB_IES |= 0x10;   // Interrupt on high-to-low transition
     Joystick_PB_IE &= ~0x10;    // disable interrupt on P2.7

     Joystick_PB_IFG = 0x00;  // Clear all P1 interrupt flags
     Joystick_PB_REN |= 0x10;   // Enable pull-up resistor on P1.4

     LCD_DATA_WRITE &= ~0xF0;
     LCD_DATA_DIR |= 0xF0;                                // P2.4-P2.7 To Output('1')
     LCD_DATA_SEL &= ~0xE0;                               // Bit clear P1.5-P1.7
     LCD_CTL_SEL  &= ~0xE0;

   }
/*
void    lcd_inito(){
    //check_connections

    LCD_DATA_WRITE &= ~0xF0;
    LCD_DATA_DIR |= 0xF0;                                // P2.4-P2.7 To Output('1')
    LCD_DATA_SEL &= ~0xE0;                               // Bit clear P1.5-P1.7
    LCD_CTL_SEL  &= ~0xE0;
        // Bit clear P1.5-P1.7
}
*/

//******************************************************************
//     LCD FUNC
//******************************************************************

void lcd_cmd(unsigned char c){ // send a command to the LCD
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
// initialize the LCD
//******************************************************************

void LCD_init(){

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
//******************************************************************
// lcd strobe functions
//******************************************************************

void lcd_strobe(){
  LCD_EN(1);
  asm("nOp");
  asm("Nop");
  LCD_EN(0);
}




void ADC10_ON(){
    ADC10CTL0 |= ADC10ON;
}

void ADC10_OFF(){
    ADC10CTL0 &= ~ADC10ON;
}


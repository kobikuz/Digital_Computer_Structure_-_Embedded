#include  <msp430.h>
#include  "bsp.h"
/*
 * bsp.c
 *

 */

void GPIO_init(){

    //RGB
    RGBSEL &= ~0x07; // as GPIO
    RGBDIR |= 0x07;  // as GPIO-output
    RGBOUT = 0x00;

    //Buzzer
    BuzzerSEL |= BIT4; // as OUTMOD A1
    BuzzerDIR |= BIT4;  // as GPIO-output

    //LDR
    LDRSEL |= BIT3;    //as ADC
    LDRDIR &= ~BIT3;     // as ADC-input

    //UART
    UARTSEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2 = TXD
    P1SEL2 = BIT1 + BIT2;
}

void sys_init(){

    WDTCTL = WDTHOLD | WDTPW;                            // stop watchdog timer

    LCD_DATA_WRITE &= ~0xFF;
    LCD_DATA_DIR |= 0xF0;                                // P1.4-P1.7 To Output('1')
    LCD_DATA_SEL &= ~0xE0;                               // Bit clear P2.5-P2.7
    LCD_CTL_SEL  &= ~0xE0;                               // Bit clear P2.5-P2.7

    TA0CTL = TASSEL_2 + MC_0 + ID_3;                     //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8

    //ADC
     ADC10CTL0 = ADC10SHT_2 + ADC10ON + SREF_0 + ADC10IE;// 16 * ADCLK + Turn on, set ref to Vcc and Gnd/
     ADC10CTL1 = INCH_3 + ADC10SSEL_3;                   // Input A3 and SMCLK
     ADC10AE0 |= BIT3;                                   // P1.3 ADC option select

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

   }


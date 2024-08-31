/*
 * hal.c
 *
 */

#include  <msp430.h>
#include  "bsp.h"
#include  "hal.h"
#include "api.h"

#define LCD_WAIT DelayMs(5)


//******************************************************************
//     VARIABLES
//******************************************************************

int selection = 8;
int flag = 0;
int x = 500;
char txt[] = "Hello World";

//******************************************************************
//     REAL_TIME_TASK
//******************************************************************


//******************************************************************
//     FUNCTIONS
//******************************************************************

void lpm(int lvl){
    switch(lvl){
    int x;
    case 0:
        _BIS_SR(LPM0_bits + GIE);
        //x++;
        break;
    case 1:
        _BIS_SR(LPM1_bits + GIE);
        //x++;
        break;
    case 2:
        _BIS_SR(LPM2_bits + GIE);
        //x++;
        break;

    }
}

int get_adc(){
    ADC10CTL0 |= ENC + ADC10SC; //Enable conversion , start conversion
    lpm(1);
    return ADC10MEM;
}

void wakeup(){
    _BIC_SR(LPM4_bits);
}
void wakeup9(){
    _BIC_SR(LPM1_bits);
}

void RGB_OUT(char color){
    RGBOUT = color;
}


void delayTimer(int x){
    while(x>1000){
        delayXms(1000);
        x = x-1000;
    }
    delayXms(x);
}

void delayXms(int Xms){
    TA0CTL = TACLR;
    TA0CTL = TASSEL_2 + MC_3 + ID_3;  //  select: 2 - SMCLK ; control: 3 - Up/Down  ; divider: 3 - /8
    TACCR0 = (65535*Xms)/1000;
    TA0CTL |= TAIE;
    //TA0CCTL0 |= CCIE;
    lpm(1);
}

void start_pwm(int pwm_period){
    int time1 = 1040000 / (pwm_period);
    TA1CTL = TASSEL_2 + MC_1;               // SMCLK ,up-mode
    TA1CCR0 = time1;
    TA1CCR2 = (int) time1 / 2;
    TA1CCTL2 |= OUTMOD_6;
}


void stop_pwm(void){
  TA1CCTL2 &= OUTMOD_0;
}

//******************************************************************
//     UART Functions
//******************************************************************

void UART_EN_TX(void){
    IE2 |= UCA0TXIE;                           // UCA0 transmit interrupt enabled
}

void UART_DIS_TX(void){
    IE2 &= ~UCA0TXIE;                           // UCA0 transmit interrupt enabled
}


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
// Delay usec functions
//******************************************************************

void DelayUs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}

//******************************************************************
// Delay msec functions
//******************************************************************

void DelayMs(unsigned int cnt){

    unsigned char i;
        for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

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


//******************************************************************
// DMA Functions
//******************************************************************



//******************************************************************
//     INTERRUPTS
//******************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A1_VECTOR
__interrupt void Timer0_A1 (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
    TA0CTL &= ~TAIE;
    _BIC_SR_IRQ(LPM1_bits);

}

#pragma vector = ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    _BIC_SR_IRQ(LPM1_bits);
}

// USCI A0/B0 Transmit ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    char * temp = txt;
    while(*temp){
        while (!(IFG2&UCA0TXIFG));
        UCA0TXBUF = *temp;
        temp++;
    }
    UART_DIS_TX();
    selection = 8;
    _BIC_SR_IRQ(LPM1_bits);

}

// USCI A0/B0 Receive ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(selection == 4) {
        if(UCA0RXBUF == 'x'){
            selection = 8;
            _BIC_SR_IRQ(LPM1_bits);
        }
        else
            x = x* 10 + UCA0RXBUF - '0';
    }


    else if (selection == 9) {
         rt_lcd[idx9] = UCA0RXBUF; //check if minus 0
         if (rt_lcd[idx9] == '.'){
             flag9 =1;

         }
         idx9++;

        //UART_EN_TX();                   // TX -> RXed character
        //char * temp = txt;
         //   while(*temp){
           //     while (!(IFG2&UCA0TXIFG));
           //     UCA0TXBUF = *temp;
            //    temp++;
          //  }
    }
    else {
        selection = UCA0RXBUF - '0';
        if(selection == 4) x = 0;
        else _BIC_SR_IRQ(LPM1_bits);
    }
}

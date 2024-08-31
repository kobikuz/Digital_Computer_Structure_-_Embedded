/*
 * hal.c
 *
 *  Created on: 11 במאי 2023
 *      Author: hagoz
 */

#include  <msp430.h>
#include  "bsp.h"
#include  "hal.h"
#include  "api.h"
#include <math.h>
#include <stdio.h>

// Define the maximum number of files
#define MAX_FILES 3

#define LCD_WAIT DelayMs(5)


//******************************************************************
//     VARIABLES
//******************************************************************
int file_pointer =0;
int recv_counter =0;
int file_read_counter =0;


int state = 8;
int flag = 0;
int x = 500;
float result_A6;
float result_A7;
int adc_res[16];

char angle_to_send [3];
int  k = 0;


int Script_mode_counter =0;
int Script_op =0;

int Script_read_counter;

//******************************************************************
//     Script
//******************************************************************


TextFiles myFiles = { .num_files = 0,
                      .file_names = {"script_0.txt","script_1.txt","script_2.txt"},
                      .file_sizes ={0,0,0}
};





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
    case 3:
        _BIS_SR(LPM3_bits + GIE);
        //x++;
        break;
    }
}

int get_adc(){
    ADC10CTL0 |= ENC+ADC10SC; //Enable conversion , start conversion
    lpm(1);
    return ADC10MEM;
}

void wakeup(){
    _BIC_SR(LPM4_bits);
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
// PB
//******************************************************************
void enable_pb(){
     Joystick_PB_IE |= 0x10;    // Enable interrupt on P2.7
}

void disable_pb(){
    Joystick_PB_IE &= ~0x10;    // diss interrupt on P2.7
}

//******************************************************************
// STEPPER
//******************************************************************
void move_counter_wise(void){
    if (curr_location <= 0){
        curr_location=510;
    }
    Stepper_Phase_OUT |= BIT0;
    //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT0;
    Stepper_Phase_OUT |= BIT3;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT3;
    Stepper_Phase_OUT |= BIT2;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT2;
    Stepper_Phase_OUT |= BIT1;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT1;
    curr_location--;
}

void move_clockwise(void){
    if (curr_location >= 510) {
        curr_location =0;
    }
    Stepper_Phase_OUT |= BIT3;
    //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT3;
    Stepper_Phase_OUT |= BIT0;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT0;
    Stepper_Phase_OUT |= BIT1;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT1;
    Stepper_Phase_OUT |= BIT2;
        //CLOCK
    delayXms(5);
    Stepper_Phase_OUT &= ~BIT2;
    curr_location++;
}
void collect_vals(){
    ADC10CTL0 &= ~ENC;
       while (ADC10CTL1 & ADC10BUSY);               // Wait if ADC10 core is active
       ADC10SA = adc_res;                        // Data buffer start
       ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion ready
       __bis_SR_register(LPM1_bits);        // LPM0, ADC10_ISR will force exit
       __no_operation();                       // space for debugger
       __no_operation();
}

void send_angle(){
    angle_to_send[0] = curr_location/100 + '0';
    angle_to_send[1] = (curr_location/10)%10 + '0';
    angle_to_send[2] = curr_location%10 + '0';
    k = 0;
    UART_EN_TX();

}

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

    if (state ==2){
    UCA0TXBUF = send_vals[send_i];
    send_i++;
    if (send_i == 9) { // TX over?
      IE2 &= ~UCA0TXIE;
      send_i =0;// Disable USCI_A0 TX interrupt
      _BIC_SR_IRQ(LPM1_bits);
        }
    }
    else if (state ==4){
        if(Script_op == 6 ){
            UCA0TXBUF = angle_to_send[k];
            k++;
            if (k == 3){
                k=0;
                IE2 &= ~UCA0TXIE;
                _BIC_SR_IRQ(LPM1_bits);
            }
        }
        else if (step_Script6==0){
            UCA0TXBUF = 'b';
            IE2 &= ~UCA0TXIE;
            }
        else{
            IE2 &= ~UCA0TXIE;
            _BIS_SR_IRQ(LPM1_bits);
        }
    }

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
    if(UCA0RXBUF == '~'){
        lcd_cmd(0x02);
        lcd_cmd(0x01);
        state = 8;
        Script_op =9;
        UART_DIS_TX();
        myFiles.num_files = 0;
        myFiles.file_sizes[0] = 0;
        myFiles.file_sizes[1] = 0;
        myFiles.file_sizes[2] = 0;
        int i;
        int j;
        for (j=0;j<2;j++){
            for(i=0;i<59;i++){
            myFiles.file_starts[j][i]=0;
            }
        }
        _BIC_SR_IRQ(LPM1_bits);
    }
    else if(state == 2) { // save for Script
        if(UCA0RXBUF == 'x'){
            UART_EN_TX(); //enable TX IE
          //  _BIC_SR_IRQ(LPM1_bits);
        }
        else if(UCA0RXBUF == 'y'){
            UART_DIS_TX(); //dissbale TX IE
            _BIC_SR_IRQ(LPM1_bits);
        }
        else if (UCA0RXBUF == '1' ||UCA0RXBUF == '3' || UCA0RXBUF == '4'){
            state = UCA0RXBUF - '0';
            _BIC_SR_IRQ(LPM1_bits);
        }
    }
    else if (state == 4) {
        if (UCA0RXBUF == 'z'){ // end

            myFiles.num_files ++;
            myFiles.file_starts[file_pointer][recv_counter] = UCA0RXBUF;
            recv_counter=0;
            file_pointer = (file_pointer + 1) % 3;

            _BIC_SR_IRQ(LPM1_bits); // wake up, sending ended
            UART_EN_TX(); //enable TX IE
        }
        else if (UCA0RXBUF == 'q'){
            if (myFiles.file_starts[file_read_counter][Script_read_counter] != 'z'){
            Script_op = myFiles.file_starts[file_read_counter][Script_read_counter]-'0';
            Script_read_counter++;
            _BIC_SR_IRQ(LPM1_bits);
        }
            else {
                Script_read_counter=0;
                file_read_counter = (file_read_counter + 1) % 3;
            }
        }
        else if (UCA0RXBUF != 'q') { // regular sends
        myFiles.file_starts[file_pointer][recv_counter] = UCA0RXBUF;
        recv_counter++;
        myFiles.file_sizes[file_pointer]++;
         }
    }
    else if (UCA0RXBUF == '1' ||UCA0RXBUF == '3' || UCA0RXBUF == '4' || UCA0RXBUF == '2'){
        state = UCA0RXBUF - '0';
        _BIC_SR_IRQ(LPM1_bits);
    }


}


/////////////////////////////////////////////////
#pragma vector=PORT1_VECTOR
  __interrupt void JOYSTICK_SW_ISR(void){
    int i;
    if (state ==3){
    if (Joystick_PB_IFG & 0x10){
        if (selector ==0){
            selector = 1; //case of Calibrate
          }
          else if (selector ==1) {
              selector = 0;
          }
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
          while(!Joystick_PB_IFG & 0x10);// wait of release PB.0 button
        for(i=1000 ; i>0 ; i--); //delay, button debounce
        Joystick_PB_IFG &= ~0x10;

    }
    }
    if (state == 2){
        if (Joystick_PB_IFG & 0x10){
            if (Painermode == 0 ){
                Painermode = 1;
              }
          else if (Painermode == 1) {
                  Painermode=2 ;
              }
          else if (Painermode == 2) {
                  Painermode=0 ;
              }
              for(i=1000 ; i>0 ; i--);  //delay, button debounce
              while(!Joystick_PB_IFG & 0x10);// wait of release PB.0 button
            for(i=1000 ; i>0 ; i--); //delay, button debounce
            Joystick_PB_IFG &= ~0x10;
        }
    }
    _BIC_SR(GIE);
    Joystick_PB_IFG &= ~0x10;
    _BIC_SR_IRQ(LPM1_bits);    // exit LPM3 bits from 0(SR)


    //PBsArrayIntPending &= ~0x0f; // manual clear of p2.0-2 interrupt flags
  }

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void) {
    _BIC_SR_IRQ(LPM1_bits);    // exit LPM0 bits from 0(SR)

}

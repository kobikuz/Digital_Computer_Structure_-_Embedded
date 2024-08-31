
#include <msp430.h>
#include  "bsp_msp2.h"    // private library
#include  "hal.h"    // private library
#include "api.h"
#include "stdio.h"


// Global Variables

unsigned int REdge1, REdge2;
char *dozen_sec = 0x30;
char *unit_sec = 0x30;
char *dozen_min = 0x30;
char *unit_min = 0x30;
char *points = ':';
int i=0;
char *B = 'B';
int k_4 =0;

//state 1!///
void freqMeas(){
       // TIMER1_A2_config();
        lcd_clear();
        WDTCTL = WDTPW + WDTHOLD;
        float N_SMCLK;
        float freq;
        float tmp = 0;
        float SMCLK_FREQ = 1048576;   // 2^20
        unsigned int real_freq;
        char strFreq[6] = {'\0'};
        write_freq_temp_LCD();
        TA1CTL |= TASSEL_2 + MC_2 + TACLR;         //start Timer
        while(selector == 1){
            //disable_interrupts();
            strFreq[5] = '\0';   // Reset strFreq
            REdge2 = REdge1 =  0;
            TA1CCTL2 |= CCIE;                                // enable interrupt
            __bis_SR_register(LPM0_bits + GIE);              // Enter LPM0
            if(REdge1 == 0 && REdge2 == 0)  // first time
              continue;
            tmp = 1.05915;  // after calc the error
            N_SMCLK = 0.9*(REdge2 - REdge1)*tmp;
            freq = SMCLK_FREQ / N_SMCLK;       // Calculate Frequency
            real_freq = (unsigned int) freq ;
            if (real_freq == 65535)  // delete later
                continue;
            sprintf(strFreq, "%d", real_freq);
            write_freq_temp_LCD();
            lcd_home();
            lcd_cursor_right();
            lcd_cursor_right();
            lcd_cursor_right();
            lcd_cursor_right();
            lcd_puts(strFreq);

            cursor_off;
            DelayMs(500);
            enable_allpb();
        }
        TA1CTL = MC_0 ; // Stop Timer
}

//state 2//
void startTimerA0(){
    TACCR0 = 62499;  // Timer Cycles - max
    TA0CTL = TASSEL_2 + MC_3 +ID_3 + TAIE;//select: 2 - ACLK-2^15 ; control: 3 - Up/Down  ; divider: 3 - /8
    // ziv: ACLK doesn't work on our msp, so we have to use smclk and divide the freq to get to 1 sec.
    //so what i did is just use ACLK and go up/down which gives us the same value for 1sec- i think
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

void CountUP(){
    TIMER0_A0_config();
      while (selector ==2){
          checkSwitch();
          lcd_cmd(0x02);
          if(i == 0){
             char const  startWatch[] ="00:00";
             lcd_puts(startWatch);
             startTimerA0();
             startTimerA0();
             i=1;
          }
          else
          {
              if( unit_sec == 0x39){
                  dozen_sec++;
                  unit_sec ='/';
              }
              if( dozen_sec =='6'){
                  unit_min++;
                  dozen_sec ='0';
              }
              if( unit_min == 0x39){
                  dozen_min++;
                  unit_min ='/';
              }
              if( dozen_min == '6'){
                  dozen_min,unit_min,dozen_min,dozen_sec =0;
              }
          }
          unit_sec++;
          lcd_cmd(0x01);
          lcd_cmd(0x02);
          lcd_puts(&dozen_min);
          lcd_puts(&unit_min);
          lcd_puts(&points);
          lcd_puts(&dozen_sec);
          lcd_puts(&unit_sec);
          enable_allpb();
          startTimerA0();
          startTimerA0();
      }
}


void checkSwitch(void){
    if (flag == 1){ //flag 1 means stop
    _BIS_SR(LPM0_bits + GIE);
    TA0CTL = MC_0;
    }
}
//STATE 3
void tone_generator(){
    float f_out;
    float coeff;
    int real_f;
    float SMCLK_FREQ;
    unsigned int period_to_pwm;
    lcd_clear();
    TA1CTL = TASSEL_2 + MC_1;                  // SMCLK, upmode
    while(selector == 3){
        ADC10CTL0 |= ENC + ADC10SC;             // Start sampling
        __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
        ADC10CTL0 &= ~ADC10ON; // Don't get into interrupt

        unsigned int adc_conv = ADC10MEM;
         coeff = 1.466;  // 1500/1023
         f_out = 1.4 * adc_conv + 1000;  //  Linear Mapping given by hanan
         real_f= (int) f_out;

         SMCLK_FREQ = 1048576; // SMCLK freq 2^20
         period_to_pwm = SMCLK_FREQ/real_f;

        TA1CCR0 = period_to_pwm;
        TA1CCR1 = (int) period_to_pwm/2;
        enable_allpb();

//calc Nout
        float m = 3.3/3.65; //Scalling
        float n =0;//no Offset
        float Nout = m*adc_conv +n;
    }
    TA1CTL = MC_0 ; // Stop Timer
}


//real_time
void startTimerA0_4(){
    TACCR0 = 50000;  // Timer Cycles - max
    TA0CTL = TASSEL_2 + MC_3 +ID_3 + TAIE;//select: 2 - ACLK-2^15 ; control: 3 - Up/Down  ; divider: 3 - /8
    // ziv: ACLK doesn't work on our msp, so we have to use smclk and divide the freq to get to 1 sec.
    //so what i did is just use ACLK and go up/down which gives us the same value for 1sec- i think
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

    void real_time(){
        TIMER0_A0_config();
        lcd_cmd(0x02);
        int temp;
        int j;
        while(selector ==4){
            temp = k_4;
            if (k_4>31){
                k_4=0;
            }
            else if(k_4>15){
                temp = temp - 16;
                lcd_cmd(0xC0);
            }

            for(j=0;j<temp;j++){
               if (k_4 !=0){
                   lcd_cursor_right();
               }
            }
            lcd_puts(&B);
            enable_allpb();
            startTimerA0();
            startTimerA0_4();
            k_4++;
            lcd_clear();
/*
            if (k_4>31){
                lcd_cmd(0x02);
                k_4=0;
                continue;
            }
            if (k_4>15){
                lcd_cmd(0xC0);
                for(j=0;j<k_4-16;j++){
                    lcd_cursor_right();
                }
                continue;
            }
            for(j=0;j<k_4;j++){
                lcd_cursor_right();
            }
            */



        }
    }


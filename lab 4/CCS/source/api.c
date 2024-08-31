/*
 * api.c
 *

 */

#include  "api.h"
#include  "hal.h"
#include  "bsp.h"
#include  <msp430.h>

int n = 0;
char rt_lcd[16];
int idx9= 0 ;
int flag9=0;

void BlinkRGB(int x)
{
    char color = 0;
    while (color < 8)
    {
        RGB_OUT(color);
        delayTimer(x);
        color++;
    }
    RGB_OUT(0);
}

void Counter(int x)
{
    char value[4] = "000\0";
    lcd_clear();
    lcd_cursor_right();
    lcd_cursor_right();
    lcd_cursor_right();
    while (selection == 2)
    {
        lcd_cursor_left();
        lcd_cursor_left();
        lcd_cursor_left();
        value[2] = n % 10 + '0';
        value[1] = (n / 10) % 10 + '0';
        value[0] = n / 100 + '0';
        lcd_puts(value);
        n++;
        delayTimer(x);
        if (n == 1000)
            n = 0;
    }
    lcd_clear();
}

void CircBuzzer(int x)
{
    int tone_series[] = { 1000, 1250, 1500, 1750, 2000, 2250, 2500, 0 };
    int * tone = tone_series;
    while (selection == 3 && *tone)
    {
        start_pwm(*tone);
        tone++;
        delayTimer(x);
    }
    stop_pwm();
}

void LDR_VAL()
{
    char LDRString[5] = "f.ff\0";
    lcd_clear();
    lcd_cursor_right();
    lcd_data('.');
    lcd_cursor_right();
    lcd_cursor_right();
    while (selection == 5)
    {
        int LDRVAL = get_adc();
        LDRVAL = (100 * (LDRVAL * 3.5)) / 1023;
        LDR_Print(LDRVAL, LDRString);
        delayTimer(50);
    }
    lcd_clear();
}

void LDR_Print(int LDRVAL, char LDRString[5])
{
    lcd_cursor_left();
    lcd_cursor_left();
    lcd_cursor_left();
    lcd_cursor_left();
    if (LDRString[0] != LDRVAL / 100 + '0')
    {
        LDRString[0] = LDRVAL / 100 + '0';
        lcd_data(LDRString[0]);
    }
    else
        lcd_cursor_right();
    lcd_cursor_right();

    if (LDRString[2] != (LDRVAL / 10) % 10 + '0')
    {
        LDRString[2] = (LDRVAL / 10) % 10 + '0';
        lcd_data(LDRString[2]);
    }
    else
        lcd_cursor_right();

    if (LDRString[3] != LDRVAL % 10 + '0')
    {
        LDRString[3] = LDRVAL % 10 + '0';
        lcd_data(LDRString[3]);
    }
    else
        lcd_cursor_right();
    delayTimer(100);
}

void ClearAll()
{
    lcd_clear();
    n = 0;
}

void timer_05()
{
    timer_set();
    lpm(1);

}

void timer_1()
{
    timer1_set();
    lpm(1);

}

void real_time(){
 //flag9=0 ;

 while (flag9 == 0){
     if (flag9==1){
         wakeup9();
     }
     lpm(1);
 }
 lcd_clear();
 lcd_puts(rt_lcd);
 lpm(1);
}

#ifndef HAL_H_
#define HAL_H_

extern int selector;
extern int flag;
extern unsigned int Count;
//extern unsigned int REdge1, REdge2;

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

extern __interrupt void Timer_A0_ISR (void);
extern __interrupt void Timer_A1_ISR (void);
extern __interrupt void Timer_A (void);
extern __interrupt void PB_ISR (void);
extern __interrupt void SW1_ISR (void);
extern __interrupt void ADC10_ISR (void);



extern void enable_allpb(void);
extern void disable_interrupts(void);
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int );
extern void lcd_strobe();
extern void lcd_init();
extern void lcd_puts(const char* );
extern void lcd_data(unsigned char);
extern void lcd_cmd(unsigned char );
extern void write_signal_shape_tmp_LCD();
extern void write_freq_temp_LCD(void);



#endif /* HAL_H_ */

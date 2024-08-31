#ifndef BSP_MSP2_H_
#define BSP_MSP2_H_

// Init Program

extern void InitGPIO(void);
extern void TIMER0_A0_config(void);
extern void TIMER1_A2_config(void);



//defines://

// PB Port

#define PBsArrayPort            P1IN
#define PBsArrayIntPending      P1IFG
#define PBsArrayIntEnable       P1IE
#define PBsArrayIntEdgeSel      P1IES
#define PBsArrayPortSel         P1SEL
#define PBsArrayPortDir         P1DIR

//Switch Port
#define SwitchSEL              P2SEL
#define SwitchDIR              P2DIR
#define SwitchOUT              P2OUT
#define SwitchIE              P2IE
#define SwitchIES               P2IES
#define SwitchIFG              P2IFG
#define SwitchREN              P2REN

#define STATE4IFG              P2IFG
#define STATE4                P2IN
#define STATE4IE            P2IE
#define STATE4IES          P2IES
#define STATE4SEL         P2SEL
#define STATE4DIR         P2DIR







/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection -- Need to understand this
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (P2OUT&=~0X20) : (P2OUT|=0X20)) // P2.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~0X20) : (P2DIR|=0X20)) // P2.5 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~0X40) : (P2OUT|=0X40)) // P2.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~0X40) : (P2DIR|=0X40)) // P2.6 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~0X80) : (P2OUT|=0X80)) // P2.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~0X80) : (P2DIR|=0X80)) // P2.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable = 4, offset to Px.4-7

#define LCD_DATA_WRITE  P1OUT
#define LCD_DATA_DIR    P1DIR
#define LCD_DATA_READ   P1IN
#define LCD_DATA_SEL    P1SEL
#define LCD_CTL_SEL     P2SEL



//lcd commands
#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm(" nop"), asm(" nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()     lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)     lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()      lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F)
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line            lcd_cmd(0xC0)



//i dont think we need it as it's already in the hal.h and the funcs are there
extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);

#endif /* BSP_MSP2_H_ */

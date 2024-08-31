/*
 * bsp.h
 *
 */

#ifndef BSP_H_
#define BSP_H_

//******************************************************************
// data matrix dim
//******************************************************************
//
//#define COLS 100
//#define ROWS 10

//******************************************************************
// Init function
//******************************************************************

extern void GPIO_init(void);
extern void sys_init(void);

//******************************************************************
// Push button
//******************************************************************
//#define PBsArrayPort            P1IN
//#define PBsArrayIntPending      P1IFG
//#define PBsArrayIntEnable       P1IE
//#define PBsArrayIntEdgeSel      P1IES
//#define PBsArrayPortSel         P1SEL
//#define PBsArrayPortDir         P1DIR

//******************************************************************
// DMA
//******************************************************************
////All Channels
//#define Dma_CTL0                DMACTL0
//#define Dma_CTL1                DMACTL1
//
//// channel 0
//#define DmaStart                DMA0SA
//#define DmaDest                 DMA0DA
//#define DmaSize                 DMA0SZ
//#define DmaCc                   DMA0CTL
//// channel 1
//#define DmbStart                DMA1SA
//#define DmbDest                 DMA1DA
//#define DmbSize                 DMA1SZ
//#define DmbCc                   DMA1CTL


//******************************************************************
// LEDS ARRAY -> P9
//******************************************************************
//
//#define LEDARRAYDIR             P9DIR
//#define LEDARRAY                P9OUT
//#define LEDARRAYSEL             P9SEL
//
//#define RGBDIR                  P4DIR
//#define RGBVALUE                P4OUT
//#define RGBSEL                  P4SEL
//


//******************************************************************
// UART -> P1.1 - RX P1.2 - TX
//******************************************************************

#define UARTDIR                  P1DIR
#define UARTOUT                  P1OUT
#define UARTSEL                  P1SEL



//******************************************************************
// RGB -> P2.0-2
//******************************************************************

#define RGBDIR                  P2DIR
#define RGBOUT                  P2OUT
#define RGBSEL                  P2SEL


//******************************************************************
// Buzzer -> P2.4
//******************************************************************

#define BuzzerDIR               P2DIR
#define BuzzerOUT               P2OUT
#define BuzzerSEL               P2SEL


//******************************************************************
// LDR -> P1.3
//******************************************************************

#define LDRDIR                  P1DIR
#define LDRIN                   P1IN
#define LDRSEL                  P1SEL

//******************************************************************
// TIMER
//******************************************************************


//******************************************************************
// KEYPAD
//******************************************************************
//
//#define KEYPADIN                P10IN
//#define KEYPADOUT               P10OUT
//#define KEYPADDIR               P10DIR
//#define KEYPADSEL               P10SEL
//
//
//#define KEYPADIRQDIR            P2DIR
//#define KEYPADIRQSEL            P2SEL
//#define KEYPADIRQEdgeSel        P2IES
//#define KEYPADIRQIntPending     P2IFG
//#define KEYPADIRQIntEnable      P2IE
//

//******************************************************************
// LCD -> P1.4-7 as DATA ; P2.5-7 as Config
//******************************************************************

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
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
#define LCD_CTL_SEL P2SEL

/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/

#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00

#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nOp"), asm("Nop"), \
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

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void LCD_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);

#endif /* BSP_H_ */

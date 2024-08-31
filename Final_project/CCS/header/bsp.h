

#ifndef BSP_H_
#define BSP_H_


//******************************************************************
// Init function
//******************************************************************

extern void GPIO_init(void);
extern void sys_init(void);
extern  void lcd_inito();
extern void ADC10_OFF();
extern void ADC10_ON();


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
// UART -> P1.1 - RX P1.2 - TX
//******************************************************************

#define UARTDIR                  P1DIR
#define UARTOUT                  P1OUT
#define UARTSEL                  P1SEL



//******************************************************************
// Joystick
//******************************************************************
#define Joystick_PB_DIR                  P1DIR
#define Joystick_PB_IN                   P1IN
#define Joystick_PB_SEL                  P1SEL
#define Joystick_PB_IES                  P1IES
#define Joystick_PB_IE                   P1IE
#define Joystick_PB_IFG                  P1IFG
#define Joystick_PB_OUT                  P1OUT
#define Joystick_PB_REN                  P1REN

//******************************************************************
// Stepper
//******************************************************************
#define Stepper_Phase_DIR                  P2DIR
#define Stepper_Phase_IN                   P2IN
#define Stepper_Phase_SEL                  P2SEL
#define Stepper_Phase_OUT                  P2OUT

//******************************************************************
// LDR -> P1.3
//******************************************************************

#define LDRDIR                  P1DIR
#define LDRIN                   P1IN
#define LDRSEL                  P1SEL

//******************************************************************
// TIMER
//******************************************************************





extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
//******************************************************************
// LCD -> P2.4-7 as DATA ; P1.5-7 as Config
//******************************************************************

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (P1OUT&=~0X20) : (P1OUT|=0X20)) // P1.5 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P1DIR&=~0X20) : (P1DIR|=0X20)) // P1.5 pin direction

#define LCD_RS(a)   (!a ? (P1OUT&=~0X40) : (P1OUT|=0X40)) // P1.6 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P1DIR&=~0X40) : (P1DIR|=0X40)) // P1.6 pin direction

#define LCD_RW(a)   (!a ? (P1OUT&=~0X80) : (P1OUT|=0X80)) // P1.7 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P1DIR&=~0X80) : (P1DIR|=0X80)) // P1.7 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable = 4, offset to Px.4-7

#define LCD_DATA_WRITE  P2OUT
#define LCD_DATA_DIR    P2DIR
#define LCD_DATA_READ   P2IN
#define LCD_DATA_SEL    P2SEL
#define LCD_CTL_SEL     P1SEL

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

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()           lcd_cmd(0x02)
#define cursor_off              lcd_cmd(0x0C)
#define cursor_on               lcd_cmd(0x0F)
#define lcd_function_set        lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line            lcd_cmd(0xC0)

extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void LCD_init();
extern void lcd_strobe();
#endif /* BSP_H_ */

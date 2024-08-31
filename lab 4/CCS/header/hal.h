/*
 * hal.h
 *
 */

#ifndef HAL_H_
#define HAL_H_

//******************************************************************
//                      extern variables
//******************************************************************
extern int selection;
extern char KB;
extern char wake_up_cause;
extern int flag;
extern int x;

//******************************************************************
//                 extern Functions
//******************************************************************

extern void lpm(int);
extern void dma_set(char *,char *,int);
extern void dma_enable(void);
extern void timer_set(void);
extern void RGB_OUT(char);
extern void wakeup9();

//******************************************************************
//                 extern interrupt routines
//******************************************************************

extern __interrupt void PB_ISR (void);
extern __interrupt void KEYPAD_ISR (void);
extern __interrupt void DMA_ISR (void);
extern __interrupt void TIMERB1_ISR(void);
extern __interrupt void TIMERB0_ISR(void);




#endif /* HAL_H_ */

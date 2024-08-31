/*
 * hal.h
 *
 *  Created on: 11 במאי 2023
 *      Author: hagoz
 */

#ifndef HAL_H_
#define HAL_H_

#define MAX_FILES 3

//******************************************************************
//                      extern variables
//******************************************************************
extern int state;
extern char KB;
extern char wake_up_cause;
extern int flag;
extern int x;
extern float result_A6;
extern float result_A7;

//******************************************************************
//                 extern Functions
//******************************************************************

extern void lpm(int);
extern void dma_set(char *,char *,int);
extern void dma_enable(void);
extern void timer_set(void);
extern void RGB_OUT(char);

extern void send_angle();
extern void enable_pb();
extern void disable_pb();
extern void move_clockwise();
extern void move_counter_wise(void);
extern void delayXms(int Xms);
extern void DelayUs(unsigned int cnt);
extern void collect_vals();
extern int adc_res[16];
extern int recv_counter ;
extern int Script_op ;
extern int Script_read_counter;
extern int file_pointer;
extern int file_read_counter;


extern int get_adc();
//******************************************************************
//                 extern interrupt routines
//******************************************************************

extern __interrupt void DMA_ISR (void);
extern __interrupt void TIMERB1_ISR(void);
extern __interrupt void TIMERB0_ISR(void);
extern __interrupt void Timer0_A1 (void);
extern __interrupt  void JOYSTICK_SW_ISR(void);
extern __interrupt void ADC10_ISR(void);

typedef struct {
    int num_files;                           // Number of files
    const char *file_names[MAX_FILES];       // Array of pointers to the names of the files
    char file_starts[MAX_FILES][60];      // Array of pointers to the start of each file
    int file_sizes[MAX_FILES];            // Array of the sizes of the files
} TextFiles;

extern TextFiles myFiles;


#endif /* HAL_H_ */

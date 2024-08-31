#ifndef _hal_H_
#define _hal_H_

extern int selector;
extern int flag;
extern char curr_num ;
extern char shift_num;

extern void LEDsArrayToggle(char);
extern void LEDsArraySet(char);
extern void LEDsArrayShow(char);
extern void LEDsArrayClear(char);
extern void enable_pb1(void);
extern void no_interpts(void);
extern void enable_allpb(void);

extern __interrupt void PB_ISR (void);

#endif

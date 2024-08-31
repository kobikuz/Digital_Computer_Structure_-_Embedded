

#ifndef API_H_
#define API_H_

extern void BlinkRGB(int );
extern  void Counter(int );
extern void CircBuzzer(int );
extern void LDR_VAL();
extern void LDR_Print(int , char* );
extern void ClearAll();
extern void timer_05();
extern void timer_1();

extern void real_time();
extern int idx9;
extern int flag9;

extern char rt_lcd[16];

#endif /* API_H_ */

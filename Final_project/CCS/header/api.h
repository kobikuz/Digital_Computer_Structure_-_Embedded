

#ifndef API_H_
#define API_H_

#define THRESHOLD 10;

//extern void BlinkRGB(void);
//extern void swapMat(void);
//extern void blockTransfer(void);

extern void calibrate();
extern void Manual_control();
extern void   Painter();
extern void Script_mode();

extern float calculate_angle(float result_A6,float result_A7);
extern int valid_bit;
extern int adc10flag;
extern int Calibrate_counter;
extern int selector;
extern int curr_location;
extern int  adc_res[16];
extern int Painermode;
extern char send_vals[10];
extern int send_i;
extern int delay ;
extern int first_down_lcd ;
extern int step_Script6;



extern  int Count_up_to;
extern int Count_down_to;
extern int Rotate_to;
extern int left_angle,right_angle,new_angle;
#endif /* API_H_ */

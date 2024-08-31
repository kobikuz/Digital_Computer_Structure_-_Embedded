#ifndef _api_H_
#define _api_H_

#include  "hal.h"     // private library - HAL layer

#define M 10
#define N 36
#define K 32



extern void enter_sen();
extern void merge_sen();
extern void shift_led();
extern void res_part2();
extern void half_sentence();

extern char data_matrix[M][N];
extern int led_arr[9];
extern char lines[5][16];
extern int curr_line;
extern char str[100];
extern char lines2[7][16];
extern char dest4[99];
extern int curr_line4;
#endif








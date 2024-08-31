/*
 * api.c
 *
 *  Created on: 11 במאי 2023
 *      Author: hagoz
 */

#include  "api.h"
#include  "hal.h"
#include  "bsp.h"
#include  <msp430.h>
#include <math.h>

int n = 0;
int selector =0;
int Calibrate_counter =0;
int num_steps=0;
int curr_location=0; //from 0 to 510
int distance;
float angle;
int new_location;
int channel=0;
float Y_value,X_value=0;
int LDRVAL;
int distance;
int Painermode =2;
char send_vals[10]; //state,Xval,Yval
int send_i =0;
int first_ind_lcd=0;
int delay =500;
char *dozen_sec = 0x30;
char *unit_sec = 0x30;
char *dozen_min = 0x30;
char *unit_min = 0x30;
char *points = ':';
int first_down_lcd;
int k_4;
int new_delay;
int num_of_steps;
int step_Script6;
int step_Script7;
char rotate_char = ' ';
char LCD_sentence[16] = "The angle is:   ";
int curr_angle;
int next_location;


int Count_up_to;
int Count_down_to;
int Rotate_to;
int left_angle,right_angle,new_angle;
char left_angle_arr[3],right_angle_arr[3];

int X_res,Y_res;

void timer_05()
{
    timer_set();
    lpm(1);

}

void timer_1()
{
    timer1_set();
    lpm(1);

}

float calculate_angle(float result_A6,float result_A7) {
    if (result_A6 == -165 & result_A7 == -165 & curr_location == 0){
        angle =0;
    }
    else if (result_A6 != 0) {  // Avoid division by zero
       // int ratio = (int)result_A7 / (int)result_A6;
        angle = atan2(-result_A6, result_A7)*(180.0 / 3.1415);  // Calculate arctangent
    } else {
        angle = (result_A7 > 0) ? 90 : -90;  // Handle special cases
    }
    return angle;
}
///////
//new funcs/
/////
void Calibrate(){
    Calibrate_counter =0;
    while (state == 3){
        enable_pb();
        lpm(1);
        while (selector == 1){
            move_clockwise();
            Calibrate_counter++;

        }
    }
    curr_location = 0;
}

void Manual_control(){
    curr_location =0;
    distance =0;
    while (state == 1){
        X_res=0;
        Y_res=0;
    collect_vals();
      int i;
      for (i=0;i<4;i++){
              X_res += adc_res[i*4];
              Y_res += adc_res[i*4+3]; //samples
      }
      X_res= X_res/4;
      Y_res= Y_res/4; //memuza
      if( 250 < X_res && X_res < 800){

          X_res = 512;
      }
      if( 250 < Y_res && Y_res < 800){

          Y_res = 512;
      }
      if (250 < X_res && X_res < 800 && 250 < Y_res && Y_res < 800)
      {
          new_location = curr_location;

      }
      else{
      X_value =  100* (X_res * 3.3) / 1023; // transform into V
      Y_value =   100*(Y_res * 3.3) / 1023;
     angle = ((int)calculate_angle(X_value-165,Y_value-165)+360); //calc angle
     new_location =(((int)angle)%360) *510/360;
      }
      distance = new_location - curr_location;
      if (abs(distance)> 15){ //this is threshold!
          if ((curr_location < 255 && new_location < 255) || (curr_location > 255 && new_location > 255)){
              if (new_location> curr_location){
                  int i;
                  for (i=0;i<10;i++){
                      move_clockwise();
                  }
              }
              else{
                  int i;
                  for (i=0;i<10;i++){
                      move_counter_wise();
                  }
          }
          }
          else{
              if ( (curr_location +255)%510 > new_location){
                            int i;
                            for (i=0;i<10;i++){
                                move_clockwise();
                            }
                            }
                        else {
                            int i;
                            for (i=0;i<10;i++){
                            move_counter_wise();
                                 }
                        }
          }

      }
    }
}

void Painter(){
    Painermode =2;
    while (state == 2){
      X_res=0;
      Y_res=0;
      collect_vals();
        int i;
        for (i=0;i<4;i++){
                X_res += adc_res[i*4];
                Y_res += adc_res[i*4+3]; //samples
        }
        X_res= X_res/4;
        Y_res= Y_res/4; //average
        send_vals[0]=Painermode  + '0';
        send_vals[1]=X_res/1000 + '0';
        send_vals[2]=(X_res/100)%10  + '0';
        send_vals[3]=(X_res/10)%10  + '0';
        send_vals[4]=(X_res%10)  + '0';
        send_vals[5]=Y_res/1000  + '0';
        send_vals[6]=(Y_res/100)%10  + '0';
        send_vals[7]=(Y_res/10)%10  + '0';
        send_vals[8]=(Y_res%10)  + '0';
        send_vals[9]='~' ;
        // sent to PC
        send_i=0;
        lpm(1);

    }
}


void inc_lcd( int Count_up_to){ //add stop at Count_up_to!!!
    first_ind_lcd=0;
    dozen_min = '0';
    unit_min ='0';
    dozen_sec ='0';
    unit_sec = '0';
    lcd_cmd(0x02);
      while (Script_op ==1){
          if(first_ind_lcd == 0){
             char const  startWatch[] ="00:00";
             lcd_puts(startWatch);
             delayXms(delay);
          }
         else if (Count_up_to==first_ind_lcd){
             Script_op =8;
            break;
         }
         else
          {
              if( unit_sec == 0x39){
                  dozen_sec++;
                  unit_sec ='/';
              }
              if( dozen_sec =='6'){
                  unit_min++;
                  dozen_sec ='0';
              }
              if( unit_min == 0x39){
                  dozen_min++;
                  unit_min ='/';
              }
              if( dozen_min == '6'){
                  dozen_min,unit_min,dozen_min,dozen_sec =0;
              }
          }
          unit_sec++;
          lcd_cmd(0x01);
          lcd_cmd(0x02);
          lcd_puts(&dozen_min);
          lcd_puts(&unit_min);
          lcd_puts(&points);
          lcd_puts(&dozen_sec);
          lcd_puts(&unit_sec);
          first_ind_lcd++;
          delayXms(delay);

      }
}

void set_delay(int new_delay){
    delay = new_delay*10;
    Script_op =8;
}

void  dec_lcd(int Count_down_to){ // from Count_down_to to 0
      int i;
      int dozen_i = Count_down_to/10;
      int unity_i = Count_down_to%10;
      char dozen = dozen_i + '0';
      char unity =unity_i + '0';
      for (i =  0 ; i <= Count_down_to ; i++){
        if (Script_op == 2){
            lcd_cmd(0x02);
              char const * minstr ="00:";
              lcd_puts(minstr);
              lcd_data(dozen);
              lcd_data(unity);

              unity = unity -1;
              if( unity == 0x2F){
                unity = 0x39;
                dozen = dozen-1;
              }
              delayXms(delay);

        }
       }
      Script_op = 8;
      // awaiting outise int
      }

void rra_lcd(char Rotate_to){
        lcd_cmd(0x02);
        lcd_clear();
        int temp;
        int j;
        while(Script_op ==3){ // for 31
            temp = k_4;
            if (k_4>31){
                k_4=0;
                Script_op = 8;
            }
            else if(k_4>15){
                temp = temp - 16;
                lcd_cmd(0xC0);
            }

            for(j=0;j<temp;j++){
               if (k_4 !=0){
                   lcd_cursor_right();
               }
            }
            rotate_char = Rotate_to +'0';
            lcd_data(rotate_char);
            delayXms(delay);
            k_4++;
            lcd_clear();

        }
   }

void stepper_deg(int new_angle){

    curr_angle = curr_location/510*360; //250
    next_location = new_angle*510/360;//140
    num_of_steps = abs(curr_location-next_location);
    step_Script6 =0;
    //enable send
    UART_EN_TX();
        if ((curr_location < 255 && next_location < 255) || (curr_location > 255 && next_location > 255)){
            if (next_location> curr_location){
                for (step_Script6=0;step_Script6<num_of_steps;step_Script6++){
                    move_clockwise();
                    if (Script_op ==6){
                    send_angle();
                    lpm(1);
                    delayXms(50);
                    }
                }
            }
            else{
                for (step_Script6=0;step_Script6<num_of_steps;step_Script6++){
                    move_counter_wise();
                    if (Script_op ==6){
                    send_angle();
                    lpm(1);
                    delayXms(50);
                    }
                }
        }
        }
        else{
            if ( (curr_location +255)%510 > next_location){
                          for(step_Script6=0;step_Script6<num_of_steps;step_Script6++){
                              move_clockwise();
                              if (Script_op ==6){
                              send_angle();
                              lpm(1);
                              delayXms(50);
                              }
                          }
                          }
                      else {
                          for (step_Script6=0;step_Script6<num_of_steps;step_Script6++){
                          move_counter_wise();
                          if (Script_op ==6){
                          send_angle();
                          lpm(1);
                          delayXms(50);
                          }
                               }
                      }
        }

    Script_op =8;
}
void stepper_scan(int left_angle,int right_angle){
    step_Script7=0;
    left_angle_arr[0]= left_angle/100+'0';
    left_angle_arr[1]= (left_angle/10)%10 +'0';
    left_angle_arr[2]= left_angle%10 +'0';
    right_angle_arr[0]= right_angle/100+'0';
    right_angle_arr[1]= (right_angle/10)%10 +'0';
    right_angle_arr[2]= right_angle%10 +'0';
    stepper_deg(left_angle);// reach left angle
    //reached angle!
    lcd_puts(LCD_sentence);
    lcd_cmd(0xC0);
    lcd_data(left_angle_arr[0]);
    lcd_data(left_angle_arr[1]);
    lcd_data(left_angle_arr[2]);
    if (left_angle > right_angle ) {
        num_of_steps =( right_angle+510-left_angle)*510/360;
    }
    else {
        num_of_steps = (right_angle-left_angle)*510/360;
    }
    for (step_Script7=0;step_Script7<num_of_steps; step_Script7++){
        move_clockwise();
        //send Angle to PC
    }
    lcd_cmd(0x01);
    lcd_puts(LCD_sentence);
    lcd_cmd(0xC0);
    lcd_data(right_angle_arr[0]);
    lcd_data(right_angle_arr[1]);
    lcd_data(right_angle_arr[2]);
    Script_op =8;
}

void Script_mode(){
    Script_op = 8;
    file_read_counter =0;
    Script_read_counter=0;
    file_pointer=0;
    recv_counter=0;
    while(state == 4){
        switch(Script_op){ //selection- from PC
        case 9:
            break;
        case 8:
            lpm(1);
            break;
        case 1:
            Count_up_to = 0;
            Count_up_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            Count_up_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            Count_up_to += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            inc_lcd(Count_up_to);
            break;
        case 2:
            Count_down_to = 0;
            Count_down_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            Count_down_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            Count_down_to += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            dec_lcd(Count_down_to);
            break;
        case 3:

            Rotate_to = 0;
            Rotate_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            Rotate_to += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            Rotate_to += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            rra_lcd(Rotate_to);
            break;
        case 4:
            new_delay = 0;
            new_delay += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            new_delay += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            new_delay += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            set_delay(new_delay);

            break;
        case 5:
            lcd_clear();
            Script_op =8;
            break;
        case 6:
            lcd_clear();
            new_angle = 0;
            new_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            new_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            new_angle += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            stepper_deg(new_angle);
            break;
        case 7:
            lcd_clear();
            left_angle = 0;
            left_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            left_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            left_angle += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            new_angle = 0;
            Script_read_counter++;
            right_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0') * 100;// cast to int
            Script_read_counter++;
            right_angle += (myFiles.file_starts[file_read_counter][Script_read_counter]-'0')*10;// cast to int
            Script_read_counter++;
            right_angle += myFiles.file_starts[file_read_counter][Script_read_counter]-'0';// cast to int
            Script_read_counter++;
            stepper_scan(left_angle,right_angle);
            break;
        }
    }
}


////*

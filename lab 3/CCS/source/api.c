#include "api.h"
#include "hal.h" // private library - API layer
#include "bsp.h"
#include "stdio.h"


// Global Variables
char data_matrix[M][N] = {"An apple a day keeps the doctor away",
"climb on the bandwagon",
"Dot the i's and cross the t's",
"He who pays the piper calls the tune",
"The pen is mightier than the sword",
"The pot calling the kettle black",
"Shed crocodile tears",
"Close but no cigar",
"Cut from the same cloth",
"Strike while the iron’s hot"
};
char lines[5][16];
char lines2[7][16];
int led_arr[9] =  {128,64,32,16,8,4,23,13,40} ;


char str[100] = { "Wise machines are dramatically more valuable than machines that just store and retrieve information" };
//char half[2][45];
char dest4[99];

unsigned int *idx;
int idx3;
int curr_line =0;
int curr_line4 =0;


//-------------------------------------------------------------


//-------------------------------------------------------------
//                        Write WithK eypad - Task 1
//-------------------------------------------------------------
void enter_sen(){
   // disable_interrupts();  // Disable GIE
    StopAllTimers();    // Stop All Timers(A,B,DMA)
    lcd_clear();
    lcd_home();
    lcd_puts("Enter a sentence:");
    //enable_interrupts();  // Enable GIE
    indx = 0;
}

//-------------------------------------------------------------
//                       Merge senteces - Task 2
//-------------------------------------------------------------
void merge_sen(){
     //disable_interrupts();  // Disable GIE
     StopAllTimers();    // Stop All Timers(A,B,DMA)
     matrix_indx[0] = 0;
     matrix_indx[1] = 0;
     lcd_clear();
     cursor_off;
     lcd_home();

     lcd_puts("choose 2 numbers");
    // enable_interrupts();  // Enable GIE
     indx = 0;

}

void res_part2(){
    int i2;
    int j2;
    i2 =0;
    while (i2 < 16*5){
        if (res_set[i2]== 0){
            lines[i2/16][i2%16]= 0;
            i2++;
            break;
        }
        lines[i2/16][i2%16]= res_set[i2];
        i2++;
    }
    if (printflag == 1){
    lcd_home();
    lcd_16bit(lines[0]);
    lcd_new_line;
    lcd_16bit(lines[1]);
    }
}

// Call LCD reset for printing which song we playing
void lcd_reset(){
    lcd_clear();
    lcd_home();
}
//-------------------------------------------------------------
//                      SHIFT LED- TASK 3
//-------------------------------------------------------------
void shift_led(){
    idx3 = 9;
    while(selector ==3){
        enable_allpb();
        if (idx3 == 9){
            idx3 = 0;
        }
        dma_set((unsigned char *)&led_arr[idx3],(char *)&LEDARRAY,2);
        dma_enable3();
        startTimerB();
        idx3++;
        //LEDARRAY = led_arr[idx3]; to ensure it works proreptly
    }
}

//-------------------------------------------------------------
//                    task 4
//-------------------------------------------------------------
void half_sentence(){
    printflag=0;
    StopAllTimers();
    DMA_config();
    int i;
    dma_enable();
    dma_set(str+50,&lines2[0],16);
    dma_enable();
    dma_set(str+66,&lines2[1],16);
    dma_enable();
    dma_set(str+82,&lines2[2],16);
    dma_enable();
    dma_set(str+98,&lines2[3],1);
    dma_enable();
    dma_set(str,&lines2[3][1],15);
    dma_enable();
    dma_set(str+15,&lines2[4],16);
    dma_enable();
    dma_set(str+31,&lines2[5],16);
    dma_enable();
    dma_set(str+47,&lines2[6],3);
    dma_enable();
    lcd_home();
    lcd_16bit(lines2[0]);
    lcd_new_line;
    lcd_16bit(lines2[1]);

}

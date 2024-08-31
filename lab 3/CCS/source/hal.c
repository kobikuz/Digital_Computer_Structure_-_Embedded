#include "api.h"
#include "hal.h" // private library - API layer
#include "bsp.h"
// Global Variables
#include <msp430.h>


int j=0;
int selector = 0;
//unsigned int KBIFG = 0;
char KB = '~'; // start value to keypad
int flag = 0; // bool to check if we pressed the same button in the keypad
int indx = 0;
char recorder_idiom[K];
int matrix_indx[2];
int idx1;
int idx2;
char *start_add1 ;
char *start_add2;
char res_set[2*N];
char *dest_add;
int total_size2;
int total_size1;
int scrollflag;
int printflag;

//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
	GPIOconfig();
	StopAllTimers();
	lcd_init();
	lcd_clear();

}

///led//
void clearled(){
    LEDARRAY    = 0x00;
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
// 				Set Byte to Port
//--------------------------------------------------------------------
void SetByteToPort(char ch){
	PBsArrPortOut |= ch;  
} 
//--------------------------------------------------------------------
// 				Clear Port Byte
//--------------------------------------------------------------------
void clrPortByte(char ch){
	PBsArrPortOut &= ~ch;
} 
//---------------------------------------------------------------------
//            General Function - No need
//---------------------------------------------------------------------
void int2str(char *str, unsigned int num){
    int strSize = 0;
    long tmp = num, len = 0;
    int j;
    // Find the size of the intPart by repeatedly dividing by 10
    while(tmp){
        len++;
        tmp /= 10;
    }

    // Print out the numbers in reverse
    for(j = len - 1; j >= 0; j--){
        str[j] = (num % 10) + '0';
        num /= 10;
    }
    strSize += len;
    str[strSize] = '\0';
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00) 
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
void enable_allpb(void){
   PBsArrIntEn = 0x0f;
  _BIS_SR(GIE);
 }
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

void lcd_16bit (const char * s){
    int is;
    for (is =0 ;is<16 ;is++){
        lcd_data(*s++);
    }
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm(" nop");
 // asm("NOP");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//                     Polling delays
//---------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm(" nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}
//---------------**************************----------------------------
//               Interrupt Services Routines
//---------------**************************----------------------------

//*********************************************************************
//            Port1 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT1_VECTOR
  __interrupt void PB_ISR (void){
    int i;
    if (PBsArrIntPend & 0x01){
          selector = 1; //case of PB P2.0
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
          while(!PBsArrIntPend & 0x01);// wait of release PB.0 button
        for(i=1000 ; i>0 ; i--); //delay, button debounce
        PBsArrIntPend &= ~0x01;

    }

    else if(PBsArrIntPend & 0x02){
          selector = 2; //case of PB P2.1
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrIntPend & 0x02);// wait of release PB.0 button
        for(i=1000 ; i>0 ; i--); //delay, button debounce
        PBsArrIntPend &= ~0x02;

    }

    else if(PBsArrIntPend & 0x04){
          selector = 3; //case of PB P2.2
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrIntPend & 0x04);// wait of release PB.0 button
        for(i=1000 ; i>0 ; i--); //delay, button debounce
        PBsArrIntPend &= ~0x04;

    }
    else if(PBsArrIntPend & 0x08){
          selector = 4; //case of PB P2.2
          for(i=1000 ; i>0 ; i--);  //delay, button debounce
           while(!PBsArrIntPend & 0x08);// wait of release PB.0 button
        for(i=1000 ; i>0 ; i--); //delay, button debounce
        PBsArrIntPend &= ~0x08;

    }
    _BIC_SR(GIE);
   // PBsArrIntEn &= ~0x0f;
    _BIC_SR_IRQ(LPM3_bits);    // exit LPM3 bits from 0(SR)


    //PBsArrayIntPending &= ~0x0f; // manual clear of p2.0-2 interrupt flags
  }
//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
 /*       switch(lpm_mode){
		case mode0:
		 LPM0_EXIT; // must be called from ISR only
		 break;
		 
		case mode1:
		 LPM1_EXIT; // must be called from ISR only
		 break;
		 
		case mode2:
		 LPM2_EXIT; // must be called from ISR only
		 break;
                 
        case mode3:
		 LPM3_EXIT; // must be called from ISR only
		 break;
                 
        case mode4:
		 LPM4_EXIT; // must be called from ISR only
		 break;
	}
        
}

*/
//*********************************************************************
//            Port2 Interrupt Service Routine
//*********************************************************************
#pragma vector=PORT2_VECTOR  // For KeyPad
  __interrupt void PBs_handler_P2(void){
      //int i;
     // for(i=1000 ; i>0 ; i--);  //delay, button debounce
        // while(!KeypadIRQIntPend & 0x02);// wait of release PB.0 button
       //for(i=1000 ; i>0 ; i--); //delay, button debounce
      delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
      if(KeypadIRQIntPend & BIT1){    // if keypad has been pressed find value
          scrollflag =0;
          KeypadPortOUT = 0x0E; //העמודות מוציאות ב0 זה יעני השורה האחרונה במטרייצה ועכשיו מתחת לזה זה שהעמודות ,טוב אחי ?
          if ( ( KeypadPortIN & 0x10 ) == 0 ){
              if (KB == 'F'){
                  KB ='Y';
                  flag = 1;
              }
              else  if (KB == 'Y'){
                  KB ='Z';
                  flag = 1;
              }
              else {
                  KB ='F';
                  flag = 0;
              }
          }
          else if ( ( KeypadPortIN & 0x20 ) == 0 ){
              scrollflag = 1;
              if (KB == 'B'){
                  KB ='W';
                  flag = 1;
              }
              else  if (KB == 'W'){
                  KB ='X';
                  flag = 1;
              }
              else {
                  KB ='B';
                  flag = 0;
              }
          }

          else if ( ( KeypadPortIN & 0x40 ) == 0 )  {
              if (KB == '0'){
                  KB ='U';
                  flag = 1;
              }
              else  if (KB == 'U'){
                              KB ='V';
                              flag = 1;
                          }
              else {
                              KB ='0';
                              flag = 0;
                          }
                      }
          else if ( ( KeypadPortIN & 0x80 ) == 0 ) {
              if (KB == 'A'){
                  KB ='S';
                  flag = 1;
              }
              else  if (KB == 'S'){
                              KB ='T';
                              flag = 1;
                          }
              else {
                              KB ='A';
                              flag = 0;
                          }
                      }

          KeypadPortOUT = 0x0D;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  {
              if (KB == 'E'){
                  KB ='R';
                  flag =1;
              }
              else {
                  KB ='E';
                  flag =0;
              }
          }
          else if ( ( KeypadPortIN & 0x20 ) == 0 ){
              if (KB == '9'){
                  KB ='Q';
                  flag = 1;
              }
              else{
                  KB ='9';
                  flag =0;
              }
          }
          else if ( ( KeypadPortIN & 0x40 ) == 0 ){
              if (KB == '8'){
                  KB ='P';
                  flag = 1;
              }
              else{
                  KB ='8';
                  flag =0;
              }
          }
          else if ( ( KeypadPortIN & 0x80 ) == 0 ){
              if (KB == '7'){
                  KB ='O';
                  flag =1;
              }
              else{
                  KB ='7';
                  flag =0;
              }
          }

          KeypadPortOUT = 0x0B;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  {
                if (KB == 'D'){
                    KB ='N';
                    flag = 1;
                }
                else{
                    KB ='D';
                    flag = 0;
                }
            }
            else if ( ( KeypadPortIN & 0x20 ) == 0 ){
                if (KB == '6') {
                    KB ='M';
                    flag = 1;
                }
                else {
                    KB ='6';
                    flag = 0;
                }
            }
            else if ( ( KeypadPortIN & 0x40 ) == 0 ){
                if (KB == '5'){
                    KB ='L';
                    flag =1;
                }
                else {
                    KB ='5';
                    flag =0;
                }
            }
            else if ( ( KeypadPortIN & 0x80 ) == 0 ){
                if (KB == '4') {
                    KB ='K';
                    flag=1;
                }
                else {
                    KB ='4';
                    flag=0;
                }
            }

          KeypadPortOUT = 0x07;
          if ( ( KeypadPortIN & 0x10 ) == 0 )  {
                if (KB == 'C'){
                    KB ='J';
                    flag =1;
                }
                else{
                    KB ='C';
                    flag =0;
                }
            }
            else if ( ( KeypadPortIN & 0x20 ) == 0 ){
                if (KB == '3'){
                    KB ='I';
                    flag=1;
                }
                else {
                    KB ='3';
                    flag =0;
                }
            }
            else if ( ( KeypadPortIN & 0x40 ) == 0 ){
                if (KB == '2'){
                    KB ='H';
                    flag=1;
                }
                else {
                    KB ='2';
                    flag =0;
                }
            }
            else if ( ( KeypadPortIN & 0x80 ) == 0 ){
                if (KB == '1'){
                    KB ='G';
                    flag =1;
                }
                else {
                    KB ='1';
                    flag =0 ;
                }
                scrollflag =0;
            }
//----------------print to lcd Task1--------------------------------------
          if (selector == 1){
              if (KB == '~' || indx == 32){
                  lcd_cmd(0x01);//lcd_clear();
                  reset_arr(recorder_idiom,K);
                  indx = 0;
              }
              else if ((KB != '~') && (flag == 0) ) {
                  if (indx == 16){
                     lcd_cmd(0xC0);
                  }
                  if (indx == 0) {
                      lcd_cmd(0x01);
                      reset_arr(recorder_idiom,K);
                  }
                  lcd_putchar(KB);
                  recorder_idiom[indx] = KB;
                  indx++;
              }
              else if(KB !='~' && flag == 1) {
                  lcd_cursor_left();
                  lcd_putchar(KB);
                  indx--;
                  recorder_idiom[indx] = KB;
                  indx++;
              }
          }
//-----------------get matrix index Task 2--------------------------------------
          if (selector == 2)
          {
              if ( scrollflag ==1 ){
                  curr_line++;
                  if(curr_line ==5){
                      curr_line =0;
                  }
                  lcd_clear();
                  lcd_home();
                  lcd_16bit(lines[curr_line]);
                  lcd_new_line;
                  lcd_16bit(lines[curr_line+1]);
                  scrollflag =0;

              }

              if ((KB >= '0' && KB <= '9') || KB == 'A'){
                  printflag == 1;
                  if(indx ==0){
                      reset_arr_int(matrix_indx,2);
                      lcd_cmd(0x01);
                  }
                  if(indx < 2 && KB != 'A'){
                      matrix_indx[indx] = KB - 48;
                      lcd_putchar(KB);
                      indx++;
                  }
                  else  if( indx ==2 && KB == 'A')//press on send:
                      {

                         hash(&idx1, &idx2,matrix_indx);
                         lcd_cmd(0x01);
                     //    indx = 0;
                    //     lcd_puts(res_set); // DMA on instead this line
                     //    delay(debounceVal);

                         reset_arr_int(matrix_indx,2);

                      }
                  else{
                      reset_arr(matrix_indx,2);
                      lcd_cmd(0x01);
                      indx = 0;
                  }
              }
              else{
                  //choose two diffrent numbers, there is a edge case when the user want to choose same number.
                     // lcd_cmd(0x01);
                      indx = 0;


                  scrollflag =0;
               }

              }
          if (selector == 4){
              if ( scrollflag ==1 ){
                  curr_line4++;
                  if(curr_line4 ==6){
                      curr_line4 =0;
                  }
                  lcd_clear();
                  lcd_home();
                  lcd_16bit(lines2[curr_line4]);
                  lcd_new_line;
                  lcd_16bit(lines2[curr_line4+1]);
                  scrollflag =0;

              }
              scrollflag =0;
          }


//-----------------------------------------------------------------
          delay(15000);   // For keypad debounce
          KeypadPortOUT &= ~0x0F;  // Reset Row1-4
          KeypadIRQIntPend &= ~BIT1; // Reset Flag
      }

  }


  void reset_arr(char arr[], int size){
      int i;
      for(i = 0; i<size;i++){
          arr[i]= '0' ;
      }
  }
  void reset_arr_int(int arr[], int size){
       int i;
       for(i = 0; i<size;i++){
           arr[i]= 0 ;
       }
   }
  //---------------------------------------------------------------------
  //           DMA FUNCTIONS
  //-----------------------------------------------------
  void dma_set(char * start_adress,char * dest_adress,int size){
      DmaStart = start_adress;
      DmaDest  = dest_adress;
      DmaSize  = size;
  }

  void dma_enable(){
      DMA0CTL |= DMAEN;
      DMA0CTL |= DMAREQ;
  }

  void dma_enable3(){
      DMA0CTL |= DMAEN;

  }
 void hash(int *id1, int *id2, int mat[2]){
     printflag =1;
       *id1 = mat[0];
       *id2 = mat[1];
       int i;
       int size;
       int f1, f2;
       start_add1 = &data_matrix[*id1];
       start_add2 = &data_matrix[*id2];
       dest_add = &res_set;
       total_size1 =0;
       total_size2 =0;
       while((*start_add1 != 0  || *start_add2 != 0 ) && (total_size1 <= N-1 && total_size2 <= N-1)){
           size =0;
           f1,f2 =0;
           while  (*start_add1 != ' ' && *start_add1 != 0 && total_size1 <= N-1){
               start_add1++;
               size++;
               f1 = 1;
               total_size1++;
           }

           if (f1 ==1){
           dma_enable();
           dma_set(start_add1-size,dest_add,size);
           dma_enable();
           dest_add=dest_add+size;
           *dest_add++ = ' '; // new 2 lines
              start_add1++;
              total_size1++;
           }
           f1=0;
           size =0;

           while (*start_add2 != ' ' && *start_add2 != 0 && total_size2 <= N-1){
               start_add2++;
               size++;
               f2=1;
               total_size2++;
           }

           if (f2 ==1){
           dma_enable();
           dma_set(start_add2-size,dest_add,size);
           dma_enable();
           dest_add=dest_add+size;
           *dest_add++ = ' '; // new 2 lines
              start_add2++;
              total_size2++;
           }
           f2=0;
           size =0;
       }
       *dest_add = '\0'; // Change 13: Null-terminate the destination buffer
       printflag =1;
   }
//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
     /* switch(lpm_mode){
      case mode0:
          LPM0_EXIT; // must be called from ISR only
          break;

      case mode1:
          LPM1_EXIT; // must be called from ISR only
          break;

      case mode2:
          LPM2_EXIT; // must be called from ISR only
          break;

      case mode3:
          LPM3_EXIT; // must be called from ISR only
          break;

      case mode4:
          LPM4_EXIT; // must be called from ISR only
          break;
      }
  */


//*********************************************************************
//            TIMER B0 ISR
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMERB0_ISR (void) {
    TBCCTL0 &= ~CCIFG;         // Clear interrupt flag
    TBCCTL0 &= ~CCIE;          // Disable interrupt (if needed)
    _BIC_SR_IRQ(LPM1_bits);    // Exit low power mode
}

// TIMER B1 ISR (for TBCCR1, TBCCR2, Overflow)
#pragma vector=TIMERB1_VECTOR
__interrupt void TIMERB1_ISR (void) {
    switch (__even_in_range(TBIV, 14)) {
        case TBIV_NONE:
            break;             // No interrupt
        case TBIV_TBCCR1:
            // Handle TBCCR1 interrupt if needed
            TBCCTL1 &= ~CCIFG; // Clear interrupt flag
            _BIC_SR_IRQ(LPM1_bits); // Exit low power mode
            break;
        case TBIV_TBCCR2:
            // Handle TBCCR2 interrupt
            TBCCTL2 &= ~CCIFG; // Clear interrupt flag
            TBCCTL2 &= ~CCIE;  // Disable interrupt (if needed)
            _BIC_SR_IRQ(LPM1_bits); // Exit low power mode
            break;
        case TBIV_TBIFG:
            // Handle overflow interrupt if needed
            // Clear overflow interrupt flag if necessary
            break;
        default:
            break;
    }
}

//*********************************************************************
//            DMA ISR
//*********************************************************************
/*#pragma vector = DMA_VECTOR
__interrupt void DMA_ISR (void){
    //StopAllTimers();
    //Show_Menu_LCD();  // Show the task 1 menu when a song finished to play
    DmaCc &= ~DMAIFG;
   // DmaCc &= ~DMAREQ;
    _BIC_SR_IRQ(LPM1_bits);
    //dma_en = 0;
    // wakeup

}
*/
#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void) {
    switch (__even_in_range(DMAIV, DMAIV_DMA2IFG)) {
        case DMAIV_DMA0IFG:
            // DMA channel 0 interrupt handler
            // Clear DMA interrupt flag if needed
            DmaCc &= ~DMAIFG;
           // DmaCc &= ~DMAREQ;
            _BIC_SR_IRQ(LPM1_bits);
            break;
        // Add cases for other DMA channels if necessary
        default:
            break;
    }
}

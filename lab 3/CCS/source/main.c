#include "api.h"
#include "hal.h" // private library - API layer
#include "bsp.h"
#include  <stdio.h>


int flag1 ;
int k;

void main(void){
  

  sysConfig();     // Configure GPIO, Stop Timers, Init LCD


  while(1){
	switch(selector){
	  case 0: //idle - Sleep
	      enable_allpb();
	      _BIS_SR(LPM3_bits + GIE);
	      int x = 1;
	      flag1 =0;
	      break;
	  case 1: //PB0 recorder
	      clearled();
	      enable_allpb();
	      enter_sen(); // Perform Notes recording - Task 1
	        enable_allpb();
	          clearled();
	      _bis_SR_register(LPM3_bits + GIE);


	    break;

	  case 2:

	      clearled();
	      printflag =0;
	      reset_arr_int(res_set,72);
	      for (k=0;k<6;k++){
	          reset_arr_int(lines[k],16);
	      }
	      enable_allpb();
	      DMA_config();
	      merge_sen();
	          _bis_SR_register(LPM3_bits + GIE);
	        res_part2();
	        clearled();
	          enable_allpb();
	      _bis_SR_register(LPM3_bits + GIE);
	      break;
		
      case 3: ;
      lcd_clear();
      enable_allpb();
      TIMERB_config();
      DMA_config_Task3();
        shift_led();
		break;

      case 4: //
          lcd_clear();
          enable_allpb();
          clearled();
          half_sentence();
          enable_allpb();
        _bis_SR_register(LPM3_bits + GIE);
		break;
		
	}
  }
}

  
  
  
  
  
  

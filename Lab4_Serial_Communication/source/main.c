#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum FSMstate nextstate;
enum SYSmode lpm_mode;// related to sleep mode ,

unsigned char LEDs_status_state[5]={0}; // save the last value before moving to the next state
char Flag_PB_status[5]={0}; // values: 1-first time push 0- secend time push
char start1=0;
unsigned int last_capture = 0;
unsigned int period = 0;
unsigned int frequency = 0;
unsigned char timeSec=0;
unsigned char timeMinute=0;
unsigned int tickCounter=0;
unsigned int tone_idx = 0;
unsigned char init = 0;
unsigned int NUM_TONES=6;
unsigned int tone_series_hz[] = {1000, 1250, 1500, 1750, 2000, 2250, 2500};
unsigned char msg_idx=0;
unsigned char msg_in_progress=0;

char msg_buf[MSG_BUF_SIZE];
char dataString[MSG_BUF_SIZE];


float f_out=0;//(q14_t)(1350 * Q14_ONE);
void main(void){
  
  state = state0;  // start in idle state on RESET
  nextstate=state;
  lpm_mode = mode0;     // start in idle state on RESET
  int i;

  sysConfig();



  //LEDs_status_state[state2]=1;
  
  while(1){
	switch(state){
	  case state0:
        enterLPM(lpm_mode);
        //lcd_clear();
        enable_interrupts();
		break;
		 
	  case state1:
		//disable_interrupts();
	  {
	      lcd_clear();
	      count_up();

	      break;
	  }
	  case state2:
	  {
	      //init = 1;
	      lcd_clear();
	      buzzer_tones();
		break;
	  }
	  case state3:
	  {
	      lcd_clear();
	      rec_X();

	      break;

	  }

	  case state4:
	  {
	      lcd_clear();
	      poten_meas();
	      break;
	  }
      case state5:
      {
          lcd_clear();
          send_love();
          break;
      }
      case state6:
      {
          lcd_clear();
          reset_count();
          break;
      }
      case state7:
      {
      lcd_clear();
      get_string();
      break;
      }
      case state8:
      {
          lcd_clear();
          break;
      }
  }
	state=nextstate;
}
}
  
  
  
  
  
  

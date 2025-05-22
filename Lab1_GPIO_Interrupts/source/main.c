#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;// related to sleep mode ,

unsigned char LEDs_status_state[5]={0}; // save the last value before moving to the next state
char Flag_PB_status[5]={0}; // values: 1-first time push 0- secend time push
char stopPWM=0;

void main(void){
  
  state = state0;  // start in idle state on RESET
  lpm_mode = mode0;     // start in idle state on RESET
  int i;
  sysConfig();
  //LEDs_status_state[state2]=1;
  
  while(1){
	switch(state){
	  case state0:
		printSWs2LEDs();
        enterLPM(lpm_mode);
		break;
		 
	  case state1:
		//disable_interrupts();
		disable_interrupts_modified(PB0);
		print2LEDs(LEDs_status_state[state1]);
		for (i=0;i<20;i++)
		{

		    if (Flag_PB_status[state1]==1)
		    {
		        incLEDs(1);
		    }
		    else
		    {
		        incLEDs(-1);
		    }
		    delay(HALF_SEC_PERIOD);	// delay of 0.5 [s]

		}
		LEDs_status_state[state1]=get_LEDs_value(); //save the last value

		enable_interrupts();
		Flag_PB_status[state1]=0; // next time start from increcing
		state=state0;
		break;
		 
	  case state2:
	    disable_interrupts_modified(PB1);
	    print2LEDs(LEDs_status_state[state2]);
		for (i=0;i<14;i++)
		{
		    shifts_LEDs(1);
		    delay(HALF_SEC_PERIOD);      // delay of 0.5 [ms]
		}

		LEDs_status_state[state2]=get_LEDs_value();
		enable_interrupts();
		state=state0;
		break;
		
	  case state3:
	  {
	      enable_interrupts();
	      unsigned int freq; //Hz
	      float dutycycles;
	      //PWM 4kHz
	      if(Flag_PB_status[state3]==1)
	      {
	          stopPWM=0;
	          createPWM_4();
	      }
	      //PWM 2kHz
	      else
	      {
	          stopPWM=0;
	          createPWM_2();

	      }

	  }


	}
  }
}
  
  
  
  
  
  

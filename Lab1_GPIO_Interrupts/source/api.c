#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer

//-------------------------------------------------------------
//            Print SWs value onto LEDs
//-------------------------------------------------------------
void printSWs2LEDs(void){
	unsigned char ch;
	
	ch = readSWs();
	print2LEDs(ch);
}
//--------------------------------------------------------------------
//            Print array to LEDs array with rate of LEDs_SHOW_RATE
//--------------------------------------------------------------------            
void printArr2SWs(char Arr[], int size, unsigned int rate){
	unsigned int i;
	
	for(i=0; i<size; i++){
		print2LEDs(Arr[i]);
		delay(rate);
	}
}

void createPWM_4()
{
        const unsigned int upTime=(PWM_PERIOD/4)*(0.75)+10; // 10 cycles for fine tuning
        const unsigned int downTime = PWM_PERIOD/4*(0.25) -10;

    while(!stopPWM)
    {
        PinOutForPwm =BIT7;
        //delay(upTime);
        __delay_cycles(upTime); //PWM_PERIOD*3/4+10

        PinOutForPwm &=~PWMon;
        PinOutForPwm &= PWMoff;
        //delay(downTime);
        __delay_cycles(downTime); //PWM_PERIOD*1/4 -10
    }

    //PinOutForPwm &=~PWMon;
    PinOutForPwm &= PWMoff;

}

void createPWM_2()
{

    const unsigned int upTime=(PWM_PERIOD/2)*(0.25)+4;  // 4 cycles for fine tuning
    const unsigned int downTime = PWM_PERIOD/2*(0.75) -4; // 4 cycles for fine tuning

    while(!stopPWM)
    {
        PinOutForPwm =BIT7;
        //delay(upTime);
        __delay_cycles(upTime); //PWM_PERIOD*3/4+10

        PinOutForPwm &=~PWMon;
        PinOutForPwm &= PWMoff;
        //delay(downTime);
        __delay_cycles(downTime); //PWM_PERIOD*1/4 -10
    }

    //PinOutForPwm &=~PWMon;
    PinOutForPwm &= PWMoff;

}

 
  


#include  "../header/api.h"    		// private library - API layer
#include  "../header/app.h"    		// private library - APP layer

enum FSMstate state;
enum FSMstate nextstate;
enum FSMstate poststate;
enum SYSmode lpm_mode;// related to sleep mode ,
char start1=0;
uint8_t last_capture = 0;


unsigned char timeSec=0;
uint16_t num=0;
uint8_t tickCounter=0;
uint8_t tone_idx = 0;
unsigned char init = 0;
extern uint16_t LDR1[LDR_CALIB_COUNT]={0};
extern uint16_t LDR2[LDR_CALIB_COUNT]={0};
int8_t currentFile=0;

servo_sweep_t servo_status={0, // running
                            0, // done
                            0, //cur
                            0, //target
                            3, // step
                            0, //target_left
                            0};//target_right



uint8_t new_state6 = 0;
uint8_t new_state7 = 0;
uint8_t stepConfigurtionLDR=0;
//Full Scan UltaSonic

void main(void){
  
  state = state0;  // start in idle state on RESET
  nextstate=state;
  lpm_mode = mode0;     // start in idle state on RESET
  sysConfig();
  fs_format();
  fs_init();       /* Mini-FAT */
  fs_up_reset();

  //LEDs_status_state[state2]=1;
  
  while(1){
	switch(state){
	  case state0:
        enterLPM(lpm_mode);
        //lcd_clear();
        enable_interrupts();
		break;
		 
	  case Increment_LCD_State:
		//disable_interrupts();
	  {
	      lcd_clear();
	      count_up();

	      break;
	  }
	  case Decrement_LCD_State:
	  {
	      //init = 1;
	      lcd_clear();
	      count_down();
	      //buzzer_tones();
		break;
	  }
	  case Rorate_Right_LCD_State:
	  {
	      lcd_clear();

	      rotate_right_char();

	      break;

	  }

	  case Set_Delay_State:
	  {
	      lcd_clear();
	      //poten_meas();
	      rec_X();
	      break;
	  }
      case Clear_LCD_State:
      {
          lcd_clear();
          nextstate=Script_state;
          break;
      }
      case Servo_Degree_State:
      {
          lcd_clear();
          //move_servo_to();
          //servo_test_sequence();
          state_distance_measure();
          break;
      }
      case Servo_Scan_State:
      {
          lcd_clear();
          ///get_string();
          servo_scan();
      break;
      }
      case Sleep_State:
      {
          lcd_clear();
          nextstate= state0;
          break;
      }
      case LDR_Conf_State:
          {
              lcd_clear();
              LDR_conf_mode();
              break;
          }
      case Get_all_LDR_calibration:
      {
          GetAll_LDR_calibration();
         break;
      }
      case Full_Scan_Ultasonic_State:
      {
          lcd_clear();
          FullScanUltraSonic();
          break;
      }
      case Full_Scan_LDR_State:
      {
          lcd_clear();
          FullScanLDR();
          break;
      }
      case Full_Scan_Mix_State:
      {
          lcd_clear();
          FullScanMix();
          break;
      }
      case ReadFiles_state:
      {
          lcd_clear();
          read_files_from_flash();
          break;
      }
      case ReadText_state:
      {
          lcd_clear();
          read_text_file_from_flash();
          break;
      }
      case Script_state:
      {
          lcd_clear();
          script_mode ();
      }




  }
	state=nextstate;
}
}
  
  
  
  
  
  

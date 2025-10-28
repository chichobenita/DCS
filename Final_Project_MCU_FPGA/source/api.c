#include  "../header/api.h"    		// private library - API layer
//#include  "../header/halGPIO.h"     // private library - HAL layer
#include "../header/script.h"
#include <string.h>        /* strncpy, memcpyï¿½   */
#include  "../header/fs.h"
//#include  "../header/ldr.h"

/* ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ */
uint8_t  crc8(const uint8_t* p, uint16_t n);
void     api_ack(uint8_t code);
void     api_nack(uint8_t code);
void     api_send_bytes(const uint8_t *p, uint8_t len);
void     api_send_start(void);
void     api_send_end(void);

/* ï¿½ï¿½ï¿½ Callback ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½-LIST ï¿½ï¿½ï¿½ */
static void api_send_fat_entry(const fs_entry_t *e){
    uint16_t size  = fs_e_size(e);
    uint8_t  type  = fs_e_type(e);
    uint16_t start = e->start;   // ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ (ï¿½2) ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½-FILE_AREA_START

    uint8_t pkt[8] = {
        e->meta,                          // [0]
        type,                             // [1]
        (uint8_t)(size & 0xFF),           // [2] size LSB
        (uint8_t)(size >> 8),             // [3] size MSB
        (uint8_t)(start & 0xFF),          // [4] start LSB
        (uint8_t)(start >> 8),            // [5] start MSB
        e->crc8,                          // [6]
        0x00                              // [7] Reserved (ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½/ï¿½ï¿½ï¿½ï¿½)
    };
    api_send_bytes(pkt, sizeof pkt);
}

//


static uint16_t buffer_a[ULTRASONIC_NUM_SAMPLES];
static uint16_t buffer_b[ULTRASONIC_NUM_SAMPLES];

static volatile uint8_t active_buffer = 0; // 0 = A ï¿½×›×ª×™×‘×”, 1 = B

uint16_t* get_write_buffer()
{
    return (active_buffer == 0) ? buffer_a : buffer_b;
}

uint16_t* get_read_buffer()
{
    return (active_buffer == 0) ? buffer_b : buffer_a;
}

void swap_buffers()
{
    active_buffer ^= 1; // ×”×—ï¿½×¤×” ×‘×™ï¿½ A ï¿½Ö¾B
}










void count_up()
{
    uint16_t max=num;
    char prev_str[3] = "000";
        char curr_str[3];

        lcd_clear();
        //lcd_puts();

        unsigned int i = 0;

        setupTimerA0Ms(delayMs);   // X = ×–ï¿½ï¿½ ×‘×™ï¿½ ×¦×¢×“×™ï¿½ (×‘ï¿½×™ï¿½×™×©× ×™×•×ª)
        startTimerA0();

        while ((state == Increment_LCD_State) && (nextstate == Increment_LCD_State) && i <= max)
        {
            while (tickCounter) // ×›ï¿½×•ï¿½×¨ ×”×’×™×¢ "×˜×¨×™×’×¨"
            {
                tickCounter--;

                sprintf(curr_str, "%d", i);
                int j;
                for (j = 0; j < 3; j++) {
                    if (curr_str[j] != prev_str[j]) {
                        lcd_cursor(j);
                        lcd_putchar(curr_str[j]);
                        prev_str[j] = curr_str[j];
                    }
                }
                if (i>=max){
                    tickCounter = 0;
                    goto DONE;
                }
                i++;

            }

            __bis_SR_register(LPM0_bits + GIE);
        }
    DONE:
        stopTimerA0();                    // çåáä ìòöåø
        // äòáø ùìéèä çæøä ìîøéõ äñ÷øéôè
        nextstate = Script_state;         // àå state0 àí àúä ìà áîöá “ñ÷øéôè”
        // àí àúä îéã îáöò îòáø îöá áìåìàä äøàùéú: state = nextstate;
    }

void updateClock()
{

}

void count_down()
{
        char prev_str[4];
        char curr_str[4];

        lcd_clear();
        sprintf(curr_str, "%d", num);
        lcd_puts(curr_str);
        strcpy(prev_str, curr_str);

        uint16_t i = (uint16_t)num;

        setupTimerA0Ms(delayMs);
        startTimerA0();

        while ((state == Decrement_LCD_State) && (nextstate == Decrement_LCD_State) && i >= 0)
        {
            while (tickCounter)
            {
                tickCounter--;

                sprintf(curr_str, "%d", i);
                int j;
                for (j = 0; j < 3; j++) {
                    if (curr_str[j] != prev_str[j]) {
                        lcd_cursor(j);
                        lcd_putchar(curr_str[j]);
                        prev_str[j] = curr_str[j];
                    }
                }

                if (i == 0)
                {
                    tickCounter = 0;
                    goto DONE;
                }
                i--;
            }

            __bis_SR_register(LPM0_bits + GIE);
        }
        DONE:
        stopTimerA0();                    // çåáä ìòöåø
        // äòáø ùìéèä çæøä ìîøéõ äñ÷øéôè
        nextstate = Script_state;         // àå state0 àí àúä ìà áîöá “ñ÷øéôè”
        // àí àúä îéã îáöò îòáø îöá áìåìàä äøàùéú: state = nextstate;
}


void rotate_right_char()
{
    setupTimerA0Ms(delayMs);
    startTimerA0();

    lcd_putchar(X);
    int i=0;
    while ((state == Rorate_Right_LCD_State) && (nextstate == Rorate_Right_LCD_State) )
            {

                while (tickCounter)
                {
                    tickCounter--;
                    lcd_display_shift();
                    i++;


                    //lcd_cursor_right();
                }
                if (i%15==0)
                {
                  lcd_home() ;

                }
                __bis_SR_register(LPM0_bits + GIE);
            }


}






// state 1
void ultrasonic_get_echo_duration_us(void) {


}
unsigned int someApiFunction(void) {
    // just read the global×’â‚¬ï¿½no wrapper function needed
    return getCapturedFrequency();
    // ×’â‚¬Â¦use f×’â‚¬Â¦
}


////////////receive X  value///////////
void rec_X(){

        lcd_puts("Delay=");
        char numbuf[5];
        sprintf(numbuf, "%d", delayMs);
        lcd_puts(numbuf);       //

        DelayMs(20000);
        nextstate = Script_state;

}




void api_send_msg(const char *msg)
{
    while (*msg)
    {

        while (*msg && ((head + 1U) % TX_BUF_SZ) != tail)
        {
            tx_buf[head] = *msg++;
            head = (head + 1U) % TX_BUF_SZ;
        }


        __disable_interrupt();
        if (!(IE2 & UCA0TXIE) && (head != tail)) {
            UCA0TXBUF = tx_buf[tail];
            tail = (tail + 1U) % TX_BUF_SZ;
            IE2 |= UCA0TXIE;
        }
        __enable_interrupt();


        if (*msg)
            __bis_SR_register(LPM0_bits + GIE);
    }


    __bis_SR_register(LPM0_bits + GIE);
}

void api_send_bytes(const uint8_t* data, uint8_t len)
{   uint8_t i;
    for (i = 0; i < len; i++) {

        while (((head + 1U) % TX_BUF_SZ) == tail);
        tx_buf[head] = data[i];
        head = (head + 1U) % TX_BUF_SZ;

        __disable_interrupt();
        if (!(IE2 & UCA0TXIE) && (head != tail)) {
            UCA0TXBUF = tx_buf[tail];
            tail = (tail + 1U) % TX_BUF_SZ;
            IE2 |= UCA0TXIE;
        }
        __enable_interrupt();
    }
}

void reset_count()
{
    timeSec    = 0;
    tickCounter = 0;
}



void send_ack(uint8_t command_id_for_Ack,uint8_t typeOfAck)
{
    uint8_t tx_buffer[MAX_BIN_MSG_LEN];
    uint8_t action[]={command_id_for_Ack,typeOfAck};
    int8_t len = build_command_message(tx_buffer,RADAR_MSG_ACK,action,2);
    if (len > 0)
    {
      api_send_bytes(tx_buffer, len);
    }
}


void api_handle_msg(const radar_message_t* msg)
{
    uint8_t cmd = msg->command_id;
        const uint8_t* payload = msg->payload;
        uint8_t len = msg->payload_len;

        switch (cmd) {
        case RADAR_CMD_INC_LCD:
            nextstate = Increment_LCD_State;
            send_ack(RADAR_CMD_INC_LCD,ACK_OK);
            if (len >= 1) num = payload[0];
            if (len >= 2) delayMs = payload[1] * 10;
            break;

        case RADAR_CMD_DEC_LCD:
            nextstate = Decrement_LCD_State;
            send_ack(RADAR_CMD_DEC_LCD,ACK_OK);
            if (len >= 1) num = payload[0];
            if (len >= 2) delayMs = payload[1] * 10;
            break;

        case RADAR_CMD_RRA_LCD:
            nextstate = Rorate_Right_LCD_State;
            send_ack(RADAR_CMD_RRA_LCD,ACK_OK);
            if (len >= 1) X = (char)payload[0];
            if (len >= 2) delayMs = payload[1] * 10;
            break;

        case RADAR_CMD_SET_DELAY:
            nextstate = Set_Delay_State;
            send_ack(RADAR_CMD_SET_DELAY,ACK_OK);
            if (len >= 1) delayMs = payload[0] * 10;
            break;

        case RADAR_CMD_CLEAR_LCD:
            nextstate = Clear_LCD_State;
            send_ack(RADAR_CMD_CLEAR_LCD,ACK_OK);
            break;

        case RADAR_CMD_SERVO_DEGREE:
            nextstate = Servo_Degree_State;
            send_ack(RADAR_CMD_SERVO_DEGREE,ACK_OK);
            if (len >= 1) {
                servo_status.target = payload[0];
                new_state6 = 0;
            }
            break;

        case RADAR_CMD_SERVO_SCAN:
            nextstate = Servo_Scan_State;
            send_ack(RADAR_CMD_SERVO_SCAN,ACK_OK);
            if (len >= 2) {
                servo_status.target_right = payload[0];
                servo_status.target_left = payload[1];
                new_state7 = 0;
            }
            break;

        case RADAR_CMD_SLEEP:
            nextstate = Sleep_State;
            send_ack(RADAR_CMD_SLEEP,ACK_OK);
            break;

        case RADAR_CMD_GET_LDR_SAMPLE:
            nextstate = state0;
            send_ack(RADAR_CMD_GET_LDR_SAMPLE,ACK_OK);
            break;

        case RADAR_CMD_LDR_CONFIGURE:
            nextstate = LDR_Conf_State;
            send_ack(RADAR_CMD_LDR_CONFIGURE,ACK_OK);
            if (len >= 1)
            {
               stepConfigurtionLDR = payload[0];
            }
            break;
        case RADAR_CMD_START_SCRICPT:
        {
            uint8_t name_id;
            name_id = payload[0] & 0x0F;
                if (script_start(name_id))
                {
                    /* òáåø ìîöá ùîôòéì step áìåìàä äøàùéú */
                    nextstate = Script_state;   /* àí éù ìê ñèééè ëæä; àçøú àôùø ìäùàéø á-state ä÷ééí */
                }
        }
                break;

            break;
        case  RADAR_CMD_LDR_GET_CALIB :
            nextstate=Get_all_LDR_calibration;
            send_ack(RADAR_CMD_LDR_GET_CALIB,ACK_OK);
            break;
        case RADAR_CMD_FULL_Scan_LDR:
            nextstate= Full_Scan_LDR_State;
            send_ack(RADAR_CMD_FULL_Scan_LDR,ACK_OK);
            break;
        case RADAR_CMD_FULL_Scan_ULTRASONIC:
            nextstate= Full_Scan_Ultasonic_State;
            send_ack(RADAR_CMD_FULL_Scan_ULTRASONIC,ACK_OK);
              break;
        case RADAR_CMD_FULL_Scan_MIX:
            nextstate= Full_Scan_Mix_State;
            send_ack(RADAR_CMD_FULL_Scan_MIX,ACK_OK);


        case RADAR_CMD_START_READ_FILE:   /* START */
               /* ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½payload: op, flags, total_hi, total_lo, crc16_lo, crc16_hi, ... */
        {

               uint8_t op     = payload[0];
               uint8_t flags  = payload[1];
               uint16_t total = (payload[2] << 8) | payload[3];
               uint16_t crc16 = payload[4] | (payload[5] << 8);
               fs_up_reset();
               if (op == OP_UPLOAD)
               {
                   uint8_t ftype = payload[6];
                   uint8_t meta  = payload[7];
                   fs_status_t st = fs_up_start(ftype, meta, total, crc16);
                   send_ack(RADAR_CMD_START_READ_FILE,st == FS_OK ? ACK_OK :
                                                      st == FS_ERR_NOSPACE ? ACK_NO_SPACE : ACK_ERR);
                   //api_ack( st == FS_OK ? ACK_OK :
                            //st == FS_ERR_NOSPACE ? ACK_NO_SPACE : ACK_ERR );
               }
               else if (op == OP_LIST)
               {
                   if (total != 0) { api_nack(ERR_FORMAT); break; }
                   fs_list(api_send_fat_entry);
                   send_ack(RADAR_CMD_START_READ_FILE,ACK_OK);
                   //api_ack(ACK_OK);
               }
               else if (op == OP_DELETE)
               {
                   if (len < 7) { send_ack(RADAR_CMD_CHUNK_READ_FILE,ERR_FORMAT); break; }
                   //api_ack( fs_delete(payload[6]) ? ACK_OK : ACK_NOT_FOUND );
                   send_ack(RADAR_CMD_START_READ_FILE, fs_delete(payload[6]) ? ACK_OK : ACK_NOT_FOUND);

               }
               else if (op == OP_REPACK)
               {
                   if (total != 0)
                   {
                       send_ack(RADAR_CMD_CHUNK_READ_FILE,ERR_FORMAT);
                       break;
                   }

                   send_ack(RADAR_CMD_START_READ_FILE, fs_repack() ? ACK_OK : ACK_ERR );
               }

               break;

        }
           case RADAR_CMD_CHUNK_READ_FILE:
           {  /* CHUNK */
               uint8_t seq = payload[0];
               uint8_t n   = payload[1];

               if (len != (uint8_t)(2 + n))
               {
                   send_ack(RADAR_CMD_CHUNK_READ_FILE,ERR_FORMAT);
                   break;
               }

               fs_status_t st = fs_up_chunk(seq, &payload[2], n);
               send_ack(RADAR_CMD_CHUNK_READ_FILE, st == FS_OK ? ACK_OK :
                                                   st == FS_ERR_SEQ ? ACK_BAD_SEQ :
                                                   st == FS_ERR_SIZE ? ERR_SIZE : ACK_ERR );
               break;
           }

           case RADAR_CMD_END_READ_FILE:
           {  /* END */
               fs_entry_t e;
               e.currentPointer=e.start;
               fs_status_t st = fs_up_end(&e);
               if (st != FS_OK)
               {
                   send_ack(RADAR_CMD_END_READ_FILE, st==FS_ERR_CRC?ERR_CRC : st==FS_ERR_SIZE?ERR_SIZE : ERR_STATE );
                   break;
               }

               /* ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ */
               if (fs_add_entry(&e)) {
                   fs_set_alloc_ptr(fs_up_dst());
                   send_ack(RADAR_CMD_END_READ_FILE,ACK_OK);
               } else {
                   send_ack(RADAR_CMD_END_READ_FILE,ERR_TABLE_FULL);
               }
               break;
           }


           default:

               break;

    }
}

void read_files_from_flash()
{
    while ((state == ReadFiles_state) && (nextstate == ReadFiles_state))
    {
            lcd_clear();
            ui_show_first10_next();
            __bis_SR_register(LPM0_bits + GIE);

    }
}

void read_text_file_from_flash()
{
    while ((state == ReadText_state) && (nextstate == ReadText_state))
        {

                lcd_clear();
                read_text_from_file();
                __bis_SR_register(LPM0_bits + GIE);

        }
}

void scriptMode()
{
    lcd_clear();
}

void get_string(void)
{
}


void FullScanLDR()
{
    lcd_puts("Full Scan LDR");
        uint8_t tx_buffer[MAX_BIN_MSG_LEN];
        new_state7 = 1;
        servo_status.target_right=0;
        servo_status.target_left=180;
        while ((state == Full_Scan_LDR_State) && (nextstate == Full_Scan_LDR_State) && (new_state7 == 1))
        {

            servo_status.cur = servo_status.target_right;
            while (servo_status.cur<=servo_status.target_left)
            {
                moveServoTo(servo_status.cur);
                //ultrasonic_get_samples_cm(); // fill the array with samples

                //build_binary_sample_message(tx_buffer, RADAR_MSG_SAMPLES_VALUES, servo_status.cur, get_write_buffer());
                //int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer());
                uint16_t ldr1_samp,ldr2_samp;
                getFullSampleOfLDR(&ldr1_samp,&ldr2_samp);
                int8_t len = build_ldr_message(tx_buffer,ldr1_samp,ldr1_samp,servo_status.cur,RADAR_MSG_SCAN_LDR_VALUES);
                if (len > 0)
                 {
                   api_send_bytes(tx_buffer, len);
                 }
                servo_status.cur+=2;//servo_status.step;
            }

            nextstate = state0;

    }
        //done full scanning
        uint8_t action[]={DONE_FULL_SCAN_LDR};
        int8_t len = build_command_message(tx_buffer,RADAR_MSG_DONE_SCANING,action,1);
        if (len > 0)
        {
           api_send_bytes(tx_buffer, len);
        }

}


void FullScanUltraSonic()
{

    lcd_puts("Full Scan dist");
    uint8_t tx_buffer[MAX_BIN_MSG_LEN];
    new_state7 = 1;
    servo_status.target_right=0;
    servo_status.target_left=180;
    while ((state == Full_Scan_Ultasonic_State) && (nextstate == Full_Scan_Ultasonic_State) && (new_state7 == 1))
    {

        servo_status.cur = servo_status.target_right;
        while (servo_status.cur<=servo_status.target_left)
        {

            moveServoTo(servo_status.cur);
            while(servo_status.running==1)
            {

            }
                ultrasonic_get_samples_cm(); // fill the array with samples



                //build_binary_sample_message(tx_buffer, RADAR_MSG_SAMPLES_VALUES, servo_status.cur, get_write_buffer());
                int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer(),RADAR_MSG_SCAN_SMPLES_VALUES);
                if (len > 0)
                {
                    api_send_bytes(tx_buffer, len);
                    swap_buffers();
                }
                servo_status.cur+=5;//servo_status.step;

        }
        nextstate = state0;

    }
    //done full scanning
    uint8_t action[]={DONE_FULL_SCAN_ULTRASONIC};
    int8_t size = build_command_message(tx_buffer,RADAR_MSG_DONE_SCANING,action,1);
    if (size > 0)
    {
       api_send_bytes(tx_buffer, size);
    }


}
void FullScanMix()
{
    lcd_puts("Full Scan Mix");
        uint8_t tx_buffer[MAX_BIN_MSG_LEN];
        new_state7 = 1;
        servo_status.target_right=0;
        servo_status.target_left=180;
        while ((state == Full_Scan_Mix_State) && (nextstate == Full_Scan_Mix_State) && (new_state7 == 1))
        {

            servo_status.cur = servo_status.target_right;
            while (servo_status.cur<=servo_status.target_left)
            {
                moveServoTo(servo_status.cur);
                ultrasonic_get_samples_cm(); // fill the array with samples

                //build_binary_sample_message(tx_buffer, RADAR_MSG_SAMPLES_VALUES, servo_status.cur, get_write_buffer());
                int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer(),RADAR_MSG_SCAN_SMPLES_VALUES);

                if (len > 0)
               {
                  api_send_bytes(tx_buffer, len);
                  swap_buffers();
               }
                uint16_t ldr1_samp,ldr2_samp;
                getFullSampleOfLDR(&ldr1_samp,&ldr2_samp);
                len = build_ldr_message(tx_buffer,ldr1_samp,ldr1_samp,servo_status.cur,RADAR_MSG_SCAN_LDR_VALUES);
                if (len > 0)
                 {
                   api_send_bytes(tx_buffer, len);
                 }
                servo_status.cur+=servo_status.step;
            }

            nextstate = state0;

    }
        //done full scanning
        uint8_t action[]={DONE_FULL_SCAN_MIX};
        int8_t len = build_command_message(tx_buffer,RADAR_MSG_DONE_SCANING,action,1);
        if (len > 0)
        {
          api_send_bytes(tx_buffer, len);
        }

}


void GetAll_LDR_calibration(void)
{
    uint8_t tx_buffer[9];

    if ((state == Get_all_LDR_calibration) && (nextstate == Get_all_LDR_calibration))
    {
        uint16_t ldr1_samp, ldr2_samp;
        uint8_t i;

        for (i = 0; i < 10; ++i)
        {
            /* 1) ÷øà ãâéîä îñ’ i îä-Flash (Info C/D) */
            ReadSampleLDR(&ldr1_samp, &ldr2_samp, i);

            /* 2) ùîåø ìîòøëéí äâìåáìééí — àí öøéê àåúí âí ìî÷åí àçø */
            LDR1[i] = ldr1_samp;
            LDR2[i] = ldr2_samp;

            /* 3) ùìç ì-PC */
            int8_t len = build_ldr_message(tx_buffer, ldr1_samp, ldr2_samp, i, RADAR_MSG_LDR_CALIB_RESULT);
            if (len > 0) {
                api_send_bytes(tx_buffer, len);
            }

            /* àôùø ìäùàéø äîúðä/ùéðä àí àöìê ðãøù øôøåù îñê/æøéîú UART */
            __bis_SR_register(LPM0_bits + GIE);
        }

        nextstate = state0;
    }
}


void LDR_conf_mode()
{
    //uint8_t tx_buffer[MAX_BIN_MSG_LEN];
    //uint16_t* buffer = get_write_buffer();
    uint8_t tx_buffer[9]; //5 for payload and 4 for header and crc

    if ((state==LDR_Conf_State)&& (nextstate==LDR_Conf_State))
    {
        lcd_puts("LDR_MODE");
          if (stepConfigurtionLDR<=9)
          {
              uint16_t ldr1_samp,ldr2_samp;
              getFullSampleOfLDR(&ldr1_samp,&ldr2_samp);
              LDR1[stepConfigurtionLDR]=ldr1_samp;
              LDR2[stepConfigurtionLDR]=ldr2_samp;

              int8_t len = build_ldr_message(tx_buffer,ldr1_samp,ldr2_samp,stepConfigurtionLDR,RADAR_MSG_LDR_DONE_SAMPLE_CALIB);
              if (len > 0)
              {
                 api_send_bytes(tx_buffer, len);
              }
              __enable_interrupt();
              __bis_SR_register(LPM0_bits + GIE);


              //build_binary_sample_message(tx_buffer, 'D', 0, (get_write_buffer()));

          }
          /* ëùñééîðå àú ëì äãâéîåú – öøéáä ì-Flash */
          if (stepConfigurtionLDR == 9) {
              ldr_calib_commit(LDR1, LDR2, 10);
              lcd_goto_xy(1,0);
              lcd_puts("CALIB SAVED  ");
              nextstate = state0;     /* çæøä ìîöá øâéì */
          }
    }
    nextstate=state0;
}


void execute_script(uint8_t file_id)
{

}

void state_distance_measure()
{

        lcd_clear();
        lcd_puts("Real-Time Mode");
        //servo_target_deg=90;
        moveServoTo(servo_status.target);
        servo_status.cur= servo_status.target;
        uint8_t tx_buffer[MAX_BIN_MSG_LEN];
        new_state6 = 1;

        if (script_running())
            {
            ultrasonic_get_samples_cm(); // fill the array with samples
            int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer(),RADAR_MSG_SAMPLES_VALUES);
                if (len > 0)
                {
                api_send_bytes(tx_buffer, len);
                swap_buffers();
                }
            nextstate = Script_state;
            }

        while ((state == Servo_Degree_State) && (nextstate == Servo_Degree_State)&& (new_state6 == 1))
        {

            ultrasonic_get_samples_cm(); // fill the array with samples


            //build_binary_sample_message(tx_buffer, RADAR_MSG_SAMPLES_VALUES,  servo_status.cur, get_write_buffer());
            int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer(),RADAR_MSG_SAMPLES_VALUES);
            if (len > 0)
            {
            api_send_bytes(tx_buffer, len);
            swap_buffers();
            }

            //LDR SAMPLE
            // Sample from LDR1
            uint16_t ldr1_samp,ldr2_samp;
            getFullSampleOfLDR(&ldr1_samp,&ldr2_samp);
            len = build_ldr_message(tx_buffer,ldr1_samp,ldr1_samp,0,RADAR_MSG_LDR_DONE_SAMPLE);
            if (len > 0)
            {
                             api_send_bytes(tx_buffer, len);
            }
         }
}






void build_binary_sample_message(uint8_t* out_buf, uint8_t type, uint8_t degree, uint16_t* samples)
{
    int i = 0;

    out_buf[i++] = '@';        //
    out_buf[i++] = type;       // ï¿½
    out_buf[i++] = degree;     // ×–×•×•×™×ª
    int j;
    for (j = 0; j < ULTRASONIC_NUM_SAMPLES; j++) {
        out_buf[i++] = (samples[j] >> 8) & 0xFF;  // MSB
        out_buf[i++] = samples[j] & 0xFF;         // LSB
    }

    out_buf[i++] = '#';        // ×¡×•×£ ×”×•×“×¢×”
}


int8_t build_sample_message(uint8_t* out_buf, uint8_t angle, const uint16_t* samples,uint8_t cmd)
{
    radar_message_t msg;
    uint8_t payload[1 + 2 * ULTRASONIC_NUM_SAMPLES];
    int i = 0;

    payload[i++] = angle;
    int j;
    for (j = 0; j < ULTRASONIC_NUM_SAMPLES; ++j) {
        payload[i++] = (samples[j] >> 8) & 0xFF;
        payload[i++] = samples[j] & 0xFF;
    }

    int res = radar_build_message(&msg, cmd, payload, i);
    if (res != 0) {
        return -1; // Error
    }

    return radar_message_to_bytes(out_buf, &msg);  // returns total length to send
}


int8_t build_ldr_message(uint8_t* out_buf, uint16_t ldr1, uint16_t ldr2,uint8_t step,uint8_t cmd)
{
    radar_message_t msg;
    uint8_t payload[5];
    payload[0]= step;
    payload[1] = (ldr1 >> 8) & 0xFF; // MSB
    payload[2] = ldr1 & 0xFF;        // LSB
    payload[3] = (ldr2 >> 8) & 0xFF;
    payload[4] = ldr2 & 0xFF;

    int8_t res = radar_build_message(&msg, cmd, payload, 5);
    if (res != 0) {
        return -1;
    }

    return radar_message_to_bytes(out_buf, &msg);
}

int8_t build_command_message(uint8_t* out_but,uint8_t cmd,const uint8_t *payload,uint8_t lenOfPayload)
{
    // this function build message for command with 1 byte of payload like ack messages
    radar_message_t msg;
    int8_t res = radar_build_message(&msg, cmd, payload,lenOfPayload);
    if (res != 0)
    {
            return -1;
    }

    return radar_message_to_bytes(out_but, &msg);
}



void api_ack(uint8_t code)
{
    uint8_t pkt[4] = { '@', 'A', code, '#' };
    api_send_bytes(pkt, 4);
}
void api_nack(uint8_t code)
{
    uint8_t pkt[4] = { '@', 'N', code, '#' };
    api_send_bytes(pkt, 4);
}
void api_send_start(void){ uint8_t b='@'; api_send_bytes(&b,1); }
void api_send_end(void)  { uint8_t b='#'; api_send_bytes(&b,1); }



void servo_scan(){

    //servo_init(400);
    lcd_puts("Servo Scan");
    uint8_t tx_buffer[MAX_BIN_MSG_LEN];
    new_state7 = 1;
    servo_status.step = 3;
    while ((state == Servo_Scan_State) && (nextstate == Servo_Scan_State) && (new_state7 == 1))
    {

        servo_status.cur = servo_status.target_right;
        while (servo_status.cur<=servo_status.target_left)
        {
            moveServoTo(servo_status.cur);
            ultrasonic_get_samples_cm(); // fill the array with samples

            //build_binary_sample_message(tx_buffer, RADAR_MSG_SAMPLES_VALUES, servo_status.cur, get_write_buffer());
            int8_t len = build_sample_message(tx_buffer, servo_status.cur, get_write_buffer(),RADAR_MSG_SAMPLES_VALUES);
            if (len > 0)
            {
              api_send_bytes(tx_buffer, len);
              swap_buffers();
           }
            servo_status.cur+=servo_status.step;
        }
        nextstate = Script_state;

            /*TA1CCR1 = pulse_0;   // ~0ï¿½
            __delay_cycles(2000000);
            TA1CCR1 = pulse_90;  // ~180ï¿½
            __delay_cycles(2000000);
            TA1CCR1 = pulse_180;  // ~90ï¿½
            __delay_cycles(2000000);*/
     }
     //servo_target_deg=90;
     //moveServoTo(servo_target_deg);


    //moveServoTo(30);
    /*startSweepOnce(30, 120, 9);   // ï¿½-30ï¿½ ï¿½ï¿½ 120ï¿½ ï¿½ï¿½ï¿½ï¿½ 9ï¿½ (ï¿½ï¿½-ï¿½ï¿½ï¿½ï¿½)
    while(state == state7 && !sweep_isDone()){
        continueSweepOnce();
    }
    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ (ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½):
    sweep_abort();*/
}


void script_mode ()
{
    if (script_running())
    {
        (void)script_step();  /* non-blocking; îøéõ ìëì äéåúø ô÷åãä àçú */
        /* àôùø ìùìá ëàï ùéðä ÷öøä / LPM / àå ìäîùéê ùàø äìåâé÷ä */
    }
    else
    {
        nextstate = state0;
    }
}






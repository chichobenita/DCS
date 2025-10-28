#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer
#include "stdio.h"
#include  "../header/app.h"
#include  "../header/radar_protocol.h"

//#define ULTRASONIC_NUM_SAMPLES 20
#define MAX_BIN_MSG_LEN RADAR_MAX_PAYLOAD_LEN + 4


extern void count_up();
extern void count_down();
extern void rotate_right_char();
extern void rec_X();



extern void reset_count();
extern void api_send_msg(const char *msg);
void api_send_bytes(const uint8_t* data, uint8_t len);
void build_binary_sample_message(uint8_t* out_buf, uint8_t type, uint8_t degree, uint16_t* samples);

int8_t build_sample_message(uint8_t* out_buf, uint8_t angle, const uint16_t* samples,uint8_t cmd);
int8_t build_ldr_message(uint8_t* out_buf, uint16_t ldr1, uint16_t ldr2 ,uint8_t step,uint8_t cmd);
int8_t build_command_message(uint8_t* out_but,uint8_t cmd,const uint8_t *payload,uint8_t lenOfPayload);
extern void send_ack(uint8_t command_id_for_Ack,uint8_t typeOfAck);
extern void api_ack(uint8_t code);
extern void get_string();



extern void  updatePWM(float f_out);

extern void  ultrasonic_get_echo_duration_us();



extern void state_distance_measure();

extern void execute_script(uint8_t file_id);


extern void api_handle_msg(const radar_message_t* msg);
extern void updateClock();
extern void GetAll_LDR_calibration();
extern void LDR_conf_mode();

extern void read_files_from_flash();

extern void read_text_file_from_flash();

extern void scriptMode();
extern void FullScanLDR();
extern void FullScanUltraSonic();
extern void FullScanMix();


/* -------------------------------------------------------- */
#define OP_UPLOAD   0x01
#define OP_LIST     0x02
#define OP_DOWNLOAD 0x03
#define OP_DELETE   0x04
#define OP_REPACK   0x05
#define OP_CAL_LDR  0x06
#define CMD_START     0x53      /* 'S' */
#define CMD_CHUNCK    0x43      /* 'C' */
#define CMD_END       0x45      /* 'E' */

#define ACK_OK         0x06
#define ACK_ERR        0x08
#define ACK_NO_SPACE   0x07
#define ACK_NOT_FOUND  0x09
#define ACK_BAD_SEQ    0x0B

#define ERR_FORMAT     0x01
#define ERR_CRC        0x02
#define ERR_SEQ        0x03
#define ERR_STATE      0x04
#define ERR_BUSY       0x05
#define ERR_OP         0x0A
#define ERR_SIZE       0x0C
#define ERR_TABLE_FULL 0x0D
/* -------------------------------------------------------- */

extern void api_nack(uint8_t code);
extern void api_send_start(void);
extern void api_send_end(void);
extern void servo_scan(void);

uint16_t* get_write_buffer(void);
uint16_t* get_read_buffer(void);
void swap_buffers(void);
#endif








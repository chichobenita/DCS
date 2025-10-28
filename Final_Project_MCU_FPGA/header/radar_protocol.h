#ifndef RADAR_PROTOCOL_H
#define RADAR_PROTOCOL_H

//#include "stdio.h"
#include <stdint.h>


#define END_MARKER   '#'
#define RADAR_MAGIC '@'
#define RADAR_MAX_PAYLOAD_LEN 25 //25

#define MAX_FILES 10

// === Command IDs ===

// From PC to MCU
extern enum RadarCommandToMCU{

        RADAR_CMD_INC_LCD                  = 0x01,
        RADAR_CMD_DEC_LCD                  = 0x02,
        RADAR_CMD_RRA_LCD                  = 0x03,
        RADAR_CMD_SET_DELAY                = 0x04,
        RADAR_CMD_CLEAR_LCD                = 0x05,
        RADAR_CMD_SERVO_DEGREE             = 0x06,
        RADAR_CMD_SERVO_SCAN               = 0x07,
        RADAR_CMD_SLEEP                    = 0x08,
        RADAR_CMD_GET_LDR_SAMPLE           = 0x09,
        RADAR_CMD_LDR_CONFIGURE            = 0x0A,
        RADAR_CMD_LDR_GET_CALIB            = 0x0B,
        RADAR_CMD_START_READ_FILE          = 0x53,
        RADAR_CMD_CHUNK_READ_FILE          = 0x43,
        RADAR_CMD_END_READ_FILE            = 0x45,
        RADAR_CMD_FULL_Scan_LDR            = 0x10,
        RADAR_CMD_FULL_Scan_ULTRASONIC     = 0x11,
        RADAR_CMD_FULL_Scan_MIX            = 0x12,
        RADAR_CMD_START_SCRICPT            = 0x13


};

extern enum actions {

    DONE_FULL_SCAN_ULTRASONIC          = 0x01,
    DONE_FULL_SCAN_LDR                 = 0x02,
    DONE_FULL_SCAN_MIX                 = 0x03

};

// From MCU to PC
extern enum RadarCommandFromMCU {

    RADAR_MSG_SAMPLES_VALUES           = 0x80,
    RADAR_MSG_LDR_DONE_SAMPLE_CALIB    = 0x81,
    RADAR_MSG_ACK                      = 0x82,
    RADAR_MSG_ERROR                    = 0x83,
    RADAR_MSG_LDR_CALIB_RESULT         = 0x84,
    RADAR_MSG_LDR_DONE_SAMPLE          = 0x85,
    RADAR_MSG_SCAN_SMPLES_VALUES       = 0x86,
    RADAR_MSG_SCAN_LDR_VALUES          = 0x87,
    RADAR_MSG_DONE_SCANING             = 0x88

};

extern enum FileSystemCommand
{
    OP_UPLOAD       =0x01,
    OP_LIST         =0x02,
    OP_DOWNLOAD     =0x03,
    OP_DELETE       =0x04,
    OP_REPACK       =0x05,
    OP_CAL_LDR      =0x06,
    CMD_START       =0x53,     /* 'S' */
    CMD_CHUNCK      =0x43,    /* 'C' */
    CMD_END         =0x45      /* 'E' */

};

extern enum AckFileSystem
{
    ACK_OK          =0x06,
    ACK_ERR         =0x08,
    ACK_NO_SPACE    =0x07,
    ACK_NOT_FOUND   =0x09,
    ACK_BAD_SEQ     =0x0B

};

extern enum ErorrsFileSystem
{
    ERR_FORMAT      =0x01,
    ERR_CRC         =0x02,
    ERR_SEQ         =0x03,
    ERR_STATE       =0x04,
    ERR_BUSY        =0x05,
    ERR_OP          =0x0A,
    ERR_SIZE        =0x0C,
    ERR_TABLE_FULL  =0x0D
};


// === Message structure ===
typedef struct {
    uint8_t magic; // always '@'
    uint8_t command_id;
    uint8_t payload_len;
    uint8_t payload[RADAR_MAX_PAYLOAD_LEN];
    uint8_t crc;
} radar_message_t;


typedef struct {
    uint8_t running;   // 1 = true ,0=false
    uint8_t done;      // 1 = true, 0= false
    uint8_t cur;       // the current degree of the motor
    uint8_t target;    // the target degree that the motor need to go
    int8_t  step;
    uint8_t target_left;
    uint8_t target_right;
} servo_sweep_t;




typedef enum {
    STATE_MAGIC,
    STATE_CMD,
    STATE_LEN,
    STATE_PAYLOAD,
    STATE_CRC,
    STATE_END
} radar_parser_state_t;


int radar_build_message(radar_message_t *msg, uint8_t command_id, const uint8_t *payload, uint8_t payload_len);
int radar_message_to_bytes(uint8_t *buffer, const radar_message_t *msg);
int radar_parse_char(radar_message_t *msg_out, uint8_t c);


#endif // RADAR_PROTOCOL_H

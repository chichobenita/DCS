#include "../header/radar_protocol.h"

#include <string.h>

static radar_parser_state_t parser_state = STATE_MAGIC;
static radar_message_t current_msg;
static uint8_t raw_buffer[1 + 1 + 1 + RADAR_MAX_PAYLOAD_LEN]; // magic + cmd + len + payload
static uint8_t index = 0;
static uint8_t payload_index = 0;

static uint8_t compute_crc(const radar_message_t *msg)
{
    uint8_t crc = 0;
    crc ^= msg->command_id;
    crc ^= msg->payload_len;
    uint8_t i;
    for ( i = 0; i < msg->payload_len; ++i)
        crc ^= msg->payload[i];
    return crc;
}

int radar_build_message(radar_message_t *msg, uint8_t command_id, const uint8_t *payload, uint8_t payload_len)
{
    if (payload_len > RADAR_MAX_PAYLOAD_LEN)
        return -1;

    msg->magic = RADAR_MAGIC;
    msg->command_id = command_id;
    msg->payload_len = payload_len;
    memcpy(msg->payload, payload, payload_len);
    msg->crc = compute_crc(msg);
    return 0;
}

int radar_message_to_bytes(uint8_t *buffer, const radar_message_t *msg)
{
    int i = 0;
    buffer[i++] = msg->magic;
    buffer[i++] = msg->command_id;
    buffer[i++] = msg->payload_len;
    uint8_t j;
    for (j = 0; j < msg->payload_len; ++j)
        buffer[i++] = msg->payload[j];
    buffer[i++] = msg->crc;
    //buffer[i++] = RADAR_END;
    return i;
}

int radar_parse_char(radar_message_t *msg_out, uint8_t c)
{
    // Always resync on magic
        if (c == RADAR_MAGIC) {
            parser_state = STATE_CMD;
            index = 0;
            payload_index = 0;
            raw_buffer[index++] = c;
            return 0;
        }

        switch (parser_state)
        {
        case STATE_CMD:
            current_msg.command_id = c;
            raw_buffer[index++] = c;
            parser_state = STATE_LEN;
            break;

        case STATE_LEN:
            current_msg.payload_len = c;
            raw_buffer[index++] = c;

            if (current_msg.payload_len > RADAR_MAX_PAYLOAD_LEN) {
                parser_state = STATE_MAGIC;
                index = 0;
                return -1;
            }

            payload_index = 0;
            if (current_msg.payload_len == 0)
                parser_state = STATE_CRC;
            else
                parser_state = STATE_PAYLOAD;
            break;

        case STATE_PAYLOAD:
            current_msg.payload[payload_index++] = c;
            raw_buffer[index++] = c;

            if (payload_index == current_msg.payload_len)
                parser_state = STATE_CRC;
            break;

        case STATE_CRC:
        {
            current_msg.crc = c;
            uint8_t computed_crc = compute_crc(&current_msg);

            parser_state = STATE_MAGIC;
            index = 0;

            if (computed_crc == current_msg.crc)
            {
                current_msg.magic = RADAR_MAGIC;
                *msg_out = current_msg;
                return 1;
            } else
            {
                return -1;
            }
        }
       }

        return 0;
    }

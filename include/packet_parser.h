#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "ring_buffer.h"


#define PACKET_START_BYTE   0xAA
#define PACKET_MAX_PAYLOAD  255

typedef enum {
    PARSER_STATE_WAIT_FOR_START,
    PARSER_STATE_READ_LENGTH,
    PARSER_STATE_READ_PAYLOAD,
    PARSER_STATE_READ_CHECKSUM,
} parser_state_t;

typedef struct {
    parser_state_t state;
    uint8_t payload[PACKET_MAX_PAYLOAD];
    uint8_t expected_length;
    uint8_t bytes_read;
    uint8_t running_checksum;
} packet_parser_t;

// Result of feeding one byte into the parser.
typedef enum {
    PARSER_RESULT_IN_PROGRESS,   // byte consumed, packet not complete yet
    PARSER_RESULT_PACKET_READY,  // a full, valid packet is now available
    PARSER_RESULT_CHECKSUM_FAIL, // packet discarded, checksum mismatch
} parser_result_t;

void parser_init(packet_parser_t *p);

// Feed one byte into the state machine.
parser_result_t parser_process_byte(packet_parser_t *p, uint8_t byte);

// Valid only immediately after PARSER_RESULT_PACKET_READY.
const uint8_t *parser_get_payload(const packet_parser_t *p);
uint8_t parser_get_payload_length(const packet_parser_t *p);

#endif // PACKET_PARSER_H
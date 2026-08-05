#include "packet_parser.h"

void parser_init(packet_parser_t *p) {
    p->state = PARSER_STATE_WAIT_FOR_START;
    p->expected_length = 0;
    p->bytes_read = 0;
    p->running_checksum = 0;
}

parser_result_t parser_process_byte(packet_parser_t *p, uint8_t byte) {
    switch (p->state) {

        case PARSER_STATE_WAIT_FOR_START:
            if (byte == PACKET_START_BYTE) {
                p->state = PARSER_STATE_READ_LENGTH;
            }
            // any other byte here is noise/garbage — silently discard and
            // stay in WAIT_FOR_START. This is what makes the parser
            // self-resynchronizing on a noisy line.
            return PARSER_RESULT_IN_PROGRESS;

        case PARSER_STATE_READ_LENGTH:
            p->expected_length = byte;
            p->bytes_read = 0;
            p->running_checksum = 0;
            // a zero-length packet has no payload bytes to read, so skip
            // straight to the checksum state
            p->state = (p->expected_length == 0)
                           ? PARSER_STATE_READ_CHECKSUM
                           : PARSER_STATE_READ_PAYLOAD;
            return PARSER_RESULT_IN_PROGRESS;

        case PARSER_STATE_READ_PAYLOAD:
            p->payload[p->bytes_read] = byte;
            p->running_checksum ^= byte;
            p->bytes_read++;
            if (p->bytes_read >= p->expected_length) {
                p->state = PARSER_STATE_READ_CHECKSUM;
            }
            return PARSER_RESULT_IN_PROGRESS;

        case PARSER_STATE_READ_CHECKSUM: {
            bool ok = (byte == p->running_checksum);
            // Always return to WAIT_FOR_START after this byte, whether the
            // checksum matched or not — either way this packet is done.
            p->state = PARSER_STATE_WAIT_FOR_START;
            return ok ? PARSER_RESULT_PACKET_READY
                      : PARSER_RESULT_CHECKSUM_FAIL;
        }

        default:
            // Unreachable in correct operation, but a defensive default
            // avoids undefined behavior if state ever gets corrupted —
            // cheap insurance in embedded code.
            p->state = PARSER_STATE_WAIT_FOR_START;
            return PARSER_RESULT_IN_PROGRESS;
    }
}

const uint8_t *parser_get_payload(const packet_parser_t *p) {
    return p->payload;
}

uint8_t parser_get_payload_length(const packet_parser_t *p) {
    return p->expected_length;
}
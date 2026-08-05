#include "unity.h"
#include "packet_parser.h"

static packet_parser_t parser;

void setUp(void) {
    parser_init(&parser);
}

void tearDown(void) {
}

// Helper: feed a full valid packet, return the result of the final byte.
static parser_result_t feed_packet(const uint8_t *payload, uint8_t len) {
    parser_result_t result = parser_process_byte(&parser, PACKET_START_BYTE);
    TEST_ASSERT_EQUAL(PARSER_RESULT_IN_PROGRESS, result);

    result = parser_process_byte(&parser, len);
    TEST_ASSERT_EQUAL(PARSER_RESULT_IN_PROGRESS, result);

    uint8_t checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        checksum ^= payload[i];
        result = parser_process_byte(&parser, payload[i]);
        TEST_ASSERT_EQUAL(PARSER_RESULT_IN_PROGRESS, result);
    }

    return parser_process_byte(&parser, checksum);
}

void test_valid_packet_is_accepted(void) {
    uint8_t payload[] = {0x01, 0x02, 0x03};
    parser_result_t result = feed_packet(payload, sizeof(payload));
    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, result);
    TEST_ASSERT_EQUAL(sizeof(payload), parser_get_payload_length(&parser));
    TEST_ASSERT_EQUAL_MEMORY(payload, parser_get_payload(&parser), sizeof(payload));
}

void test_zero_length_payload_is_valid(void) {
    parser_result_t result = feed_packet(NULL, 0);
    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, result);
    TEST_ASSERT_EQUAL(0, parser_get_payload_length(&parser));
}

void test_corrupted_checksum_is_rejected(void) {
    parser_process_byte(&parser, PACKET_START_BYTE);
    parser_process_byte(&parser, 1);      // length = 1
    parser_process_byte(&parser, 0x55);   // payload byte
    parser_result_t result = parser_process_byte(&parser, 0xFF); // wrong checksum
    TEST_ASSERT_EQUAL(PARSER_RESULT_CHECKSUM_FAIL, result);
}

void test_parser_recovers_after_checksum_failure(void) {
    // feed one corrupted packet, then a valid one right after —
    // the parser must not get stuck
    parser_process_byte(&parser, PACKET_START_BYTE);
    parser_process_byte(&parser, 1);
    parser_process_byte(&parser, 0x55);
    parser_process_byte(&parser, 0xFF); // wrong checksum, packet dropped

    uint8_t payload[] = {0x99};
    parser_result_t result = feed_packet(payload, sizeof(payload));
    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, result);
}

void test_garbage_before_start_byte_is_ignored(void) {
    // noise on the line before a real packet starts
    parser_process_byte(&parser, 0x00);
    parser_process_byte(&parser, 0xFF);
    parser_process_byte(&parser, 0x12);

    uint8_t payload[] = {0xAB, 0xCD};
    parser_result_t result = feed_packet(payload, sizeof(payload));
    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, result);
}

void test_back_to_back_packets_both_parse(void) {
    uint8_t payload1[] = {0x11};
    uint8_t payload2[] = {0x22, 0x33};

    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, feed_packet(payload1, sizeof(payload1)));
    TEST_ASSERT_EQUAL_MEMORY(payload1, parser_get_payload(&parser), sizeof(payload1));

    TEST_ASSERT_EQUAL(PARSER_RESULT_PACKET_READY, feed_packet(payload2, sizeof(payload2)));
    TEST_ASSERT_EQUAL_MEMORY(payload2, parser_get_payload(&parser), sizeof(payload2));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_valid_packet_is_accepted);
    RUN_TEST(test_zero_length_payload_is_valid);
    RUN_TEST(test_corrupted_checksum_is_rejected);
    RUN_TEST(test_parser_recovers_after_checksum_failure);
    RUN_TEST(test_garbage_before_start_byte_is_ignored);
    RUN_TEST(test_back_to_back_packets_both_parse);
    return UNITY_END();
}
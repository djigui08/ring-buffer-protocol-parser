#include "unity.h"
#include "ring_buffer.h"

static ring_buffer_t rb;

void setUp(void) {
    ring_buffer_init(&rb);
}

void tearDown(void) {
    // nothing to clean up — no dynamic allocation
}

void test_new_buffer_is_empty(void) {
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
    TEST_ASSERT_FALSE(ring_buffer_is_full(&rb));
    TEST_ASSERT_EQUAL(0, ring_buffer_count(&rb));
}

void test_push_then_pop_returns_same_byte(void) {
    TEST_ASSERT_TRUE(ring_buffer_push(&rb, 0x42));
    uint8_t out = 0;
    TEST_ASSERT_TRUE(ring_buffer_pop(&rb, &out));
    TEST_ASSERT_EQUAL_HEX8(0x42, out);
    TEST_ASSERT_TRUE(ring_buffer_is_empty(&rb));
}

void test_pop_on_empty_buffer_fails(void) {
    uint8_t out = 0;
    TEST_ASSERT_FALSE(ring_buffer_pop(&rb, &out));
}

void test_push_until_full_then_reject(void) {
    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        TEST_ASSERT_TRUE(ring_buffer_push(&rb, (uint8_t)i));
    }
    TEST_ASSERT_TRUE(ring_buffer_is_full(&rb));
    // one more push should be rejected, not overwrite data
    TEST_ASSERT_FALSE(ring_buffer_push(&rb, 0xFF));
}

void test_fifo_order_is_preserved(void) {
    ring_buffer_push(&rb, 1);
    ring_buffer_push(&rb, 2);
    ring_buffer_push(&rb, 3);
    uint8_t out;
    ring_buffer_pop(&rb, &out); TEST_ASSERT_EQUAL(1, out);
    ring_buffer_pop(&rb, &out); TEST_ASSERT_EQUAL(2, out);
    ring_buffer_pop(&rb, &out); TEST_ASSERT_EQUAL(3, out);
}

void test_wraparound_works_correctly(void) {
    // fill, drain, refill — forces head/tail to wrap past the array end,
    // which is the classic ring buffer bug source if the modulo math is wrong
    uint8_t out;
    for (int i = 0; i < RING_BUFFER_CAPACITY; i++) {
        ring_buffer_push(&rb, (uint8_t)i);
    }
    for (int i = 0; i < RING_BUFFER_CAPACITY / 2; i++) {
        ring_buffer_pop(&rb, &out);
    }
    for (int i = 0; i < RING_BUFFER_CAPACITY / 2; i++) {
        TEST_ASSERT_TRUE(ring_buffer_push(&rb, (uint8_t)(0x80 + i)));
    }
    TEST_ASSERT_TRUE(ring_buffer_is_full(&rb));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_new_buffer_is_empty);
    RUN_TEST(test_push_then_pop_returns_same_byte);
    RUN_TEST(test_pop_on_empty_buffer_fails);
    RUN_TEST(test_push_until_full_then_reject);
    RUN_TEST(test_fifo_order_is_preserved);
    RUN_TEST(test_wraparound_works_correctly);
    return UNITY_END();
}
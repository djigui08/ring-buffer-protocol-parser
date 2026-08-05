#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb->count == 0;
}

bool ring_buffer_is_full(const ring_buffer_t *rb) {
    return rb->count == RING_BUFFER_CAPACITY;
}

size_t ring_buffer_count(const ring_buffer_t *rb) {
    return rb->count;
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte) {
    if (ring_buffer_is_full(rb)) {
        return false;
    }
    rb->buffer[rb->head] = byte;
    rb->head = (rb->head + 1) % RING_BUFFER_CAPACITY;
    rb->count++;
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *out_byte) {
    if (ring_buffer_is_empty(rb)) {
        return false;
    }
    *out_byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_CAPACITY;
    rb->count--;
    return true;
}
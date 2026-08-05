#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#define RING_BUFFER_CAPACITY 256

typedef struct {
    uint8_t buffer[RING_BUFFER_CAPACITY];
    size_t head;   // next write position
    size_t tail;   // next read position
    size_t count;  // number of bytes currently stored
} ring_buffer_t;

// Resets the buffer to empty. Must be called before first use.
void ring_buffer_init(ring_buffer_t *rb);

// Pushes one byte. Returns false (and does nothing) if the buffer is full —
// callers must handle backpressure explicitly rather than silently
// overwriting data.
bool ring_buffer_push(ring_buffer_t *rb, uint8_t byte);

// Pops one byte into *out_byte. Returns false if the buffer is empty.
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *out_byte);

bool ring_buffer_is_empty(const ring_buffer_t *rb);
bool ring_buffer_is_full(const ring_buffer_t *rb);
size_t ring_buffer_count(const ring_buffer_t *rb);

#endif // RING_BUFFER_H
# Architecture

## Components

Two independent, decoupled modules:

- **`ring_buffer`** — fixed-size FIFO byte buffer. No I/O, no allocation.
- **`packet_parser`** — a pure state machine that consumes bytes one at a
  time and reports when a complete, valid packet is ready. Has no
  knowledge of *where* bytes come from — could be fed from a ring buffer,
  a UART ISR, or a unit test.

This separation mirrors the driver/business-logic split used throughout
the larger ESP32 firmware project this component feeds into.

## Packet parser state machine

\`\`\`
                 byte == START_BYTE
  WAIT_FOR_START ────────────────────> READ_LENGTH
       ^                                    │
       │ any other byte: ignored            │ store length
       │ (self-resyncing on noise)          ▼
       │                              READ_PAYLOAD ──(length == 0? skip)──┐
       │                                    │                            │
       │                    collect `length` bytes                       │
       │                                    ▼                            ▼
       └──────────────────────────── READ_CHECKSUM <────────────────────┘
              (always returns here,
               pass or fail)
\`\`\`

Every state has exactly one forward transition and, on failure, returns
cleanly to `WAIT_FOR_START` — the parser can never get permanently stuck
on corrupted input.

## Memory model

- Both modules use fixed-size, statically allocated buffers. No `malloc`/`free`.
- `ring_buffer_t` costs `RING_BUFFER_CAPACITY` (256) bytes + 3×`size_t`.
- `packet_parser_t` costs `PACKET_MAX_PAYLOAD` (255) bytes + a few small fields.
- Deterministic memory usage is a deliberate embedded-style constraint,
  even though this currently runs on a host machine.

## Concurrency

Both modules are currently single-threaded / SPSC (single-producer,
single-consumer) and **not** thread-safe. See `docs/decisions.md` for why
that's an acceptable v1 trade-off and what changes under FreeRTOS.

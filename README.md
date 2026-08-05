# Ring Buffer + Packet Parser

[![CI](https://github.com/djigui08/ring-buffer-protocol-parser/actions/workflows/ci.yml/badge.svg)](https://github.com/djigui08/ring-buffer-protocol-parser/actions/workflows/ci.yml)

A small, dependency-free C library implementing a fixed-size ring buffer
and a byte-oriented packet parser — the kind of building blocks used to
ingest data from a UART/serial line in embedded firmware.

Built as a standalone, host-testable component before integrating it into
a larger ESP32 firmware project. No hardware required to build, test, or
review this repo.

## Why this exists

Most junior embedded portfolios are either a single Arduino sketch or a
sprawling feature demo. This project is deliberately narrow: it exists to
demonstrate software architecture, testing discipline, and documentation —
the things a firmware interview actually probes for — using a component
simple enough to fully verify by hand.

See [`docs/architecture.md`](docs/architecture.md) for the design and
[`docs/decisions.md`](docs/decisions.md) for the trade-offs behind it.

## What's here

| Path | Purpose |
|---|---|
| `include/` | Public API (`ring_buffer.h`, `packet_parser.h`) |
| `src/` | Implementation |
| `tests/` | Unity-based unit tests (12 tests total, all passing in CI) |
| `.github/workflows/` | CI: build + run both test suites on every push |

## Building and running the tests

Requires `gcc` (or `clang`). No external dependencies beyond the vendored
Unity test framework in `tests/unity/`.

\`\`\`bash
# Ring buffer tests
gcc tests/test_ring_buffer.c tests/unity/unity.c src/ring_buffer.c \
    -Iinclude -Itests/unity -o test_ring_buffer -Wall -Wextra
./test_ring_buffer

# Packet parser tests
gcc tests/test_packet_parser.c tests/unity/unity.c src/packet_parser.c \
    -Iinclude -Itests/unity -o test_packet_parser -Wall -Wextra
./test_packet_parser
\`\`\`

## Protocol format

\`\`\`
[START_BYTE][LENGTH][PAYLOAD ... LENGTH bytes ...][CHECKSUM]
\`\`\`

- `START_BYTE` (`0xAA`) — resyncs the parser after noise or a mid-stream start
- `LENGTH` — 1 byte, payload is 0–255 bytes
- `CHECKSUM` — XOR of all payload bytes

## License

MIT

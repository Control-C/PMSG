#pragma once
#include <furi.h>
#include <furi_string.h>

// Very small line assembler (called from RX callback context)
// Usage:
// - uart_line_reader_push_byte(...)
// - when it returns true, read line from provided FuriString

static inline bool uart_line_reader_push_byte(FuriString* line, uint8_t b, size_t max_len) {
    if(b == '\r') return false;

    if(b == '\n') {
        return furi_string_size(line) > 0;
    }

    if(furi_string_size(line) < max_len) {
        furi_string_push_back(line, (char)b);
    } else {
        // overflow: drop line
        furi_string_reset(line);
    }
    return false;
}

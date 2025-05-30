#pragma once
#include <array>
#include <string>
#include "tdef.hpp"

// RAII class encapsulating global buffers and pointers used by troff.
class TroffProcessor {
  public:
    // Input buffer and associated pointers
    std::array<char, IBUFSZ> inputBuffer{}; // Primary input buffer
    std::array<char, IBUFSZ> extraBuffer{}; // Secondary input buffer
    char *inputPtr{inputBuffer.data()}; // Pointer into inputBuffer
    char *extraPtr{extraBuffer.data()}; // Pointer into extraBuffer
    char *endInput{inputBuffer.data()}; // End pointer for inputBuffer
    char *endExtra{extraBuffer.data()}; // End pointer for extraBuffer

    // Output buffer and pointer
    std::array<char, OBUFSZ> outputBuffer{}; // Device output buffer
    char *outputPtr{outputBuffer.data()}; // Pointer into outputBuffer

    TroffProcessor() = default; // Constructor initializes pointers
    ~TroffProcessor() = default; // Destructor performs no cleanup
};

extern TroffProcessor g_processor; // Global processor instance

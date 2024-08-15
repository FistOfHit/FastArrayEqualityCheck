#include "pairing_functions/szudzik.h"


uint64_t szudzikPairing(int32_t a, int32_t b) {
    // Map the input integers to non-negative integers
    uint64_t A = a >= 0 ? 2 * static_cast<uint64_t>(a) : -2 * static_cast<uint64_t>(a) - 1;
    uint64_t B = b >= 0 ? 2 * static_cast<uint64_t>(b) : -2 * static_cast<uint64_t>(b) - 1;

    // Calculate the unique positive integer using the Szudzik pairing function
    uint64_t C = A >= B ? A * A + A + B : A + B * B;
    
    return C;
}

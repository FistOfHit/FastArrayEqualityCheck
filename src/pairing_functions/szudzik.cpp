/**
 * @file szudzik.cpp
 * @brief Implementation of the Szudzik pairing function.
 *
 * This file contains the implementation of the Szudzik pairing function,
 * which maps two integers to a unique non-negative integer. This function
 * is useful for creating unique identifiers from pairs of integers and
 * can be used in hash tables or other data structures.
 */

#include "pairing_functions/szudzik.h"

uint64_t szudzikPairing(int32_t a, int32_t b) {
    // Map the input integers to non-negative integers
    // For non-negative x: f(x) = 2x
    // For negative x: f(x) = -2x - 1
    uint64_t A = a >= 0 ? 2 * static_cast<uint64_t>(a) : -2 * static_cast<uint64_t>(a) - 1;
    uint64_t B = b >= 0 ? 2 * static_cast<uint64_t>(b) : -2 * static_cast<uint64_t>(b) - 1;

    // Calculate the unique positive integer using the Szudzik pairing function
    // If A >= B: C = A^2 + A + B
    // Otherwise: C = A + B^2
    uint64_t C = A >= B ? A * A + A + B : A + B * B;
    
    return C;
}

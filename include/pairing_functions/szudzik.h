#ifndef SZUDZIK_H
#define SZUDZIK_H

#include <cstdint>


/**
 * @brief Computes a unique non-negative integer from two 32-bit signed
 * integers using the Szudzik pairing function.
 *
 * This function implements the Szudzik pairing function, which maps two 
 * integers (both positive and negative) into a single unique 
 * non-negative integer.
 * 
 * The Szudzik pairing function is defined as:
 *   - If A >= B: C = A * A + A + B
 *   - Otherwise: C = A + B * B
 *
 * To handle negative integers, the function first maps each input
 * integer to a 
 * unique non-negative integer using the following transformation:
 *   - For non-negative integers: A' = 2 * A
 *   - For negative integers: A' = -2 * A - 1
 *
 * @param a An int32_t signed integer.
 * @param b An int32_t signed integer.
 * @return A uint64_t non-negative integer that uniquely represents
 * the pair (a, b).
 */
uint64_t szudzikPairing(int32_t a, int32_t b);

#endif // SZUDZIK_H

#ifndef SZUDZIK_H
#define SZUDZIK_H

#include <cstdint>

/**
 * @brief Szudzik pairing function that maps two integers to a unique positive integer.
 *
 * The Szudzik pairing function is a bijective mapping from Z x Z to N, where Z is the set
 * of integers and N is the set of non-negative integers. It allows for efficient encoding
 * of pairs of integers into a single positive integer.
 *
 * @param a The first integer to be paired.
 * @param b The second integer to be paired.
 * @return The unique positive integer representing the pair (a, b).
 */
uint64_t szudzikPairing(int32_t a, int32_t b);

#endif // SZUDZIK_H

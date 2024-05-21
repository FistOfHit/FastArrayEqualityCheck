#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include <cstdint>
#include <functional>
#include <vector>


/**
 * @brief Applies a butterfly operation on a 1D array using given pairing and
 * compression functions.
 *
 * This function performs a series of butterfly operations on a 1D array of
 * integers, assuming the size of the array is a power of 2. The butterfly
 * access pattern involves accessing pairs of elements successively, i.e.
 * accessing each pair of numbers (0, 1), (2, 3), ..., (n-2, n-1), and
 * then accessing (0, 2), (4, 6), ..., (n-3, n-1) and so on, until eventually
 * only one pair remains (0, n-1). This function then applies a given
 * transform to each pair. This process is repeated for log2(size) levels,
 * halving the stride at each level.
 *
 * @param array A reference to a std::vector<int32_t> containing the input
 *     data. The input array should have a size that is a power of 2.
 * @param transform A std::function<int32_t(int32_t, int32_t)> that takes two
 *     int32_t elements and returns an int32 value
 */
void butterfly_1d(
    std::vector<int32_t>& array,
    std::function<int32_t(int32_t, int32_t)> transform
);

#endif // BUTTERFLY_H
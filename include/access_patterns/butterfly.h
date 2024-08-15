#ifndef BUTTERFLY_H
#define BUTTERFLY_H

#include <cstdint>
#include <functional>
#include <vector>


/**
 * @brief Applies a butterfly operation on a 1D array using a given transform
 * function.
 *
 * This function performs a series of butterfly operations on a 1D array of
 * integers. It assumes the size of the array is a power of 2. The butterfly
 * access pattern involves pairing elements at increasing distances, applying
 * the transform function to each pair, and storing the result.
 *
 * The process is repeated for log2(size) levels, halving the stride at each
 * level:
 * 1. Pairs (0,1), (2,3), ..., (n-2,n-1)
 * 2. Pairs (0,2), (4,6), ..., (n-4,n-2)
 * 3. And so on, until the final pair (0,n-1)
 *
 * @param array A reference to a std::vector<int32_t> containing the input
 * data. The input array should have a size that is a power of 2.The array
 * is modified in-place.
 * @param transform A std::function that takes two int32_t elements and returns
 * an int32_t value. This function is applied to each pair of elements in the
 * butterfly pattern.
 *
 * @note The size of the input array must be a power of 2. The function does
 * not check for this condition and may produce unexpected results or errors
 * if this condition is not met.
 *
 * @see szudzikPairing() for an example of a transform function that can be
 * used.
 *
 * Example usage:
 * @code
 * std::vector<int32_t> data = {1, 2, 3, 4, 5, 6, 7, 8};
 * auto sum = [](int32_t a, int32_t b) { return a + b; };
 * butterfly_1d(data, sum);
 * // data is now {36, 28, 21, 15, 10, 6, 3, 1}
 * @endcode
 */
void butterfly_1d(
    std::vector<int32_t>& array,
    std::function<int32_t(int32_t, int32_t)> transform
);

#endif // BUTTERFLY_H
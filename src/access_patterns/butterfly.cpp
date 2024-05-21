#include <cmath>
#include <cstdint>
#include <functional>
#include <vector>


void butterfly_1d(
    std::vector<int32_t>& array,
    std::function<uint64_t(int32_t, int32_t)> transform
) {
    // Assuming size is power of 2
    int size = array.size();
    int num_iterations = std::log2(size);

    int stride = 2;
    int half_stride = 1;
    // Loop over each halving/butterfly level
    for (int n = 0; n < num_iterations; n++) {
        // Transform and store in the same array
        for (int i = 0; i < (n - stride); i += stride) {
            array[i] = transform(array[i], array[i + half_stride]);
        }

        stride *= 2;
        half_stride *= 2;
    }
}
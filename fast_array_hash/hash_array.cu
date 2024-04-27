#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <vector>


/* Hashing function

Uses a simple LSH (Locality Sensitive Hashing) function to hash an integer
value.

@param value: The value to hash
@param seed: A seed value to use in the hash function

@return: The hashed value
*/
__device__ auto lsh_hash(auto value, const auto seed) {
    value ^= seed;
    value *= 0x3243f6a8885a308d;
    value ^= value >> 15;
    value *= 0x1111111111111111;
    value ^= value >> 15;

    return value;
}


/* Pairing function

Uses the Szudzik pairing function to combine two signed 32 bit integers
into a single unsigned 64 bit integer.

@param a: The first integer
@param b: The second integer

@return: The combined integer
*/
__device__ u_int64_t szudzikPairing(int32_t a, int32_t b) {
    uint64_t A = a >= 0 ? 2 * static_cast<uint64_t>(a) : -2 * static_cast<uint64_t>(a) - 1;
    uint64_t B = b >= 0 ? 2 * static_cast<uint64_t>(b) : -2 * static_cast<uint64_t>(b) - 1;
    uint64_t C = A >= B ? A * A + A + B : A + B * B;
    return C;
}


int main() {
    const auto array_size = 1 << 20;  // 1 million elements
    const auto seed = 42;

    // Allocate memory on host
    std::vector<unsigned int> host_input(array_size);
    unsigned int host_output;

    // Initialize input array
    for (auto i = 0; i < array_size; i++) {
        host_input[i] = i;
    }

    // Allocate memory on device
    unsigned int* device_input;
    unsigned int* device_output;
    cudaMalloc((void**)&device_input, array_size * sizeof(unsigned int));
    cudaMalloc((void**)&device_output, sizeof(unsigned int));

    // Copy input data from host to device
    cudaMemcpy(device_input, host_input.data(), array_size * sizeof(unsigned int), cudaMemcpyHostToDevice);

    // Launch kernel
    const auto block_size = 256;
    const auto grid_size = (array_size + block_size - 1) / block_size;
    lsh_hash_kernel<<<grid_size, block_size>>>(device_input, device_output, array_size, seed);

    // Copy output data from device to host
    cudaMemcpy(&host_output, device_output, sizeof(unsigned int), cudaMemcpyDeviceToHost);

    // Print the hash of the array
    std::cout << "Output: " << host_output << std::endl;

    // Free device memory
    cudaFree(device_input);
    cudaFree(device_output);

    return 0;
}
#include <iostream>
#include <cstdint>


/* Cantor's pairing function

Pairs two non-negative integers into a single non-negative integer.

@param x: The first non-negative integer to pair
@param y: The second non-negative integer to pair

@return: The paired non-negative integer
*/
std::uint64_t cantor_pairing(std::uint32_t x, std::uint32_t y) {
    // 8 bytes for intermediates and final result
    std::uint64_t sum = x + y;
    std::uint64_t pairing = (sum * (sum + 1)) / 2 + y;

    return pairing;
}


/* Szudzik's pairing function

Pairs two 32-bit integers into a single 64-bit integer. Converts two N-byte
integers into a single 2N-byte integer.

@param x: The first integer to pair
@param y: The second integer to pair

@return: The paired integer
*/
std::uint64_t szudzik_pairing(std::int32_t x, std::int32_t y) {
    // 4N bytes for intermediates
    std::uint64_t A = (std::int64_t)(x >= 0 ? 2 * (std::int64_t)x : -2 * (std::int64_t)x - 1);
    std::uint64_t B = (std::int64_t)(y >= 0 ? 2 * (std::int64_t)y : -2 * (std::int64_t)y - 1);

    // 2N bytes for final result
    std::int64_t C = (std::int64_t)((A >= B ? A * A + A + B : A + B * B) / 2);

    // Fix for duplication due to signs
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0)) ? C : -C - 1;
}


int main() {
    std::int32_t x, y;

    // Example 1
    x = 10;
    y = 20;
    std::cout << "Pairing (" << x << ", " << y << ") = " << szudzik_pairing(x, y) << std::endl;

    x = 20;
    y = 10;
    std::cout << "Pairing (" << x << ", " << y << ") = " << szudzik_pairing(x, y) << std::endl;

    // Example 2
    x = -5;
    y = 7;
    std::cout << "Pairing (" << x << ", " << y << ") = " << szudzik_pairing(x, y) << std::endl;

    // Example 3
    x = -12;
    y = -8;
    std::cout << "Pairing (" << x << ", " << y << ") = " << szudzik_pairing(x, y) << std::endl;

    // Example 4
    x = 0;
    y = 0;
    std::cout << "Pairing (" << x << ", " << y << ") = " << szudzik_pairing(x, y) << std::endl;

    return 0;
}

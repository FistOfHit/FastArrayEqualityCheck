// szudzik.cpp
#include <iostream>
#include <cstdint>

uint64_t szudzikPairing(int32_t a, int32_t b) {
    uint64_t A = a >= 0 ? 2 * static_cast<uint64_t>(a) : -2 * static_cast<uint64_t>(a) - 1;
    uint64_t B = b >= 0 ? 2 * static_cast<uint64_t>(b) : -2 * static_cast<uint64_t>(b) - 1;
    uint64_t C = A >= B ? A * A + A + B : A + B * B;
    return C;
}

int main() {
    // Test cases
    int32_t testCases[][2] = {
        {1, 2},
        {2, 1},
        {-3, 4},
        {5, -6},
        {-7, -8},
        {0, 0}
    };

    // Test the Szudzik pairing function
    for (const auto& testCase : testCases) {
        int32_t a = testCase[0];
        int32_t b = testCase[1];
        uint64_t pairedValue = szudzikPairing(a, b);
        std::cout << "Szudzik pairing of (" << a << ", " << b << ") = " << pairedValue << std::endl;
    }

    return 0;
}
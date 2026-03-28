#include <fae/fae.h>

#include <cstdint>
#include <iostream>
#include <span>
#include <vector>

int main() {
    const std::vector<std::int32_t> values {4, 8, 15, 16, 23, 42};
    const fae::Fingerprint fingerprint = fae::fingerprint(std::span(values));

    std::cout << "low=" << fingerprint.low << '\n';
    std::cout << "high=" << fingerprint.high << '\n';
    std::cout << "length=" << fingerprint.length << '\n';

    return 0;
}

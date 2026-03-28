#include <fae/fae.h>

#include <cstdint>
#include <iostream>
#include <span>
#include <vector>

int main() {
    const std::vector<std::uint32_t> lhs {1, 2, 3, 4};
    const std::vector<std::uint32_t> rhs {1, 2, 3, 5};

    const fae::FingerprintComparison comparison =
        fae::compare_fingerprints(std::span(lhs), std::span(rhs));

    std::cout
        << (comparison == fae::FingerprintComparison::fingerprints_match
                ? "fingerprints_match"
                : "fingerprints_differ")
        << '\n';

    return 0;
}

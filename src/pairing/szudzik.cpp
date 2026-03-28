#include "fae/pairing/szudzik.h"

namespace fae::pairing {

std::uint64_t szudzik_pair(std::int32_t lhs, std::int32_t rhs) {
    const std::uint64_t a = lhs >= 0
                                ? 2ULL * static_cast<std::uint64_t>(lhs)
                                : (-2ULL * static_cast<std::uint64_t>(lhs)) - 1ULL;
    const std::uint64_t b = rhs >= 0
                                ? 2ULL * static_cast<std::uint64_t>(rhs)
                                : (-2ULL * static_cast<std::uint64_t>(rhs)) - 1ULL;
    return a >= b ? (a * a) + a + b : a + (b * b);
}

} // namespace fae::pairing

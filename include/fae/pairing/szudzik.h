#ifndef FAE_PAIRING_SZUDZIK_H
#define FAE_PAIRING_SZUDZIK_H

#include <cstdint>

namespace fae::pairing {

/**
 * @brief Encode two signed 32-bit integers into a single non-negative
 * 64-bit integer.
 *
 * This is an exact advanced pairing primitive. It is useful for experiments,
 * algorithm evaluation, and future pairing extension work. It is not the sole
 * basis of the high-level probabilistic fingerprint API.
 */
[[nodiscard]] std::uint64_t szudzik_pair(std::int32_t lhs, std::int32_t rhs);

} // namespace fae::pairing

#endif

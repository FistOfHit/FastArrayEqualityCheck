#ifndef FAE_REDUCTION_PAIRWISE_FOLD_H
#define FAE_REDUCTION_PAIRWISE_FOLD_H

#include <cstdint>
#include <span>

namespace fae::reduction {

/**
 * @brief Fold a sequence of 64-bit values into a single 64-bit value.
 *
 * Folding is deterministic for a given input order and seed. The operation is
 * probabilistic and intended for fingerprint construction rather than exact
 * reconstruction.
 */
[[nodiscard]] std::uint64_t
pairwise_fold64(std::span<const std::uint64_t> values, std::uint64_t seed);

} // namespace fae::reduction

#endif

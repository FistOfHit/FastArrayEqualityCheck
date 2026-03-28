#ifndef FAE_FINGERPRINT_H
#define FAE_FINGERPRINT_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>
#include <vector>

namespace fae {

/**
 * @brief A deterministic probabilistic fingerprint for an array-like input.
 *
 * `Fingerprint` contains two 64-bit lanes plus the input length. Equality of
 * fingerprints indicates that two arrays produced the same output under the
 * configured algorithm. It is not documented as a proof of array equality.
 *
 * The struct is a v1 in-memory API type, not a stable serialized wire format.
 * In particular, `length` uses `std::size_t`, so callers should not persist or
 * exchange raw `Fingerprint` bytes across versions, platforms, or ABIs.
 */
struct Fingerprint {
    std::uint64_t low {};
    std::uint64_t high {};
    std::size_t length {};

    [[nodiscard]] friend constexpr bool
    operator==(const Fingerprint&, const Fingerprint&) = default;
};

/**
 * @brief Selects the fingerprint implementation used by the public API.
 *
 * `hash_combine` is the default production algorithm in v1. `legacy_fold`
 * remains available as an opt-in compatibility and comparison path, and both
 * `cantor_mix` and `tuple_mix` are available as public opt-in alternatives
 * informed by the benchmark harness.
 */
enum class FingerprintAlgorithm {
    legacy_fold,
    hash_combine,
    cantor_mix,
    tuple_mix
};

/**
 * @brief Configuration for deterministic fingerprint generation.
 *
 * The current implementation supports a single user-controlled seed and an
 * algorithm selector. The same input, seed, and algorithm always produce the
 * same fingerprint.
 */
struct FingerprintConfig {
    std::uint64_t seed {};
    FingerprintAlgorithm algorithm {FingerprintAlgorithm::hash_combine};
};

/**
 * @brief Result of comparing two generated fingerprints.
 *
 * The enum intentionally describes fingerprint behavior, not mathematical
 * truth about the original arrays.
 */
enum class FingerprintComparison {
    fingerprints_match,
    fingerprints_differ
};

namespace detail {

[[nodiscard]] Fingerprint fingerprint_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag,
    FingerprintAlgorithm algorithm
);

template <std::integral T>
[[nodiscard]] constexpr std::uint64_t normalize_integral(T value) {
    if constexpr (std::is_same_v<T, bool>) {
        return value ? 1ULL : 0ULL;
    } else {
        return static_cast<std::uint64_t>(static_cast<std::make_unsigned_t<T>>(value));
    }
}

template <std::integral T>
[[nodiscard]] constexpr std::uint64_t integral_type_tag() {
    constexpr std::uint64_t size_bits = static_cast<std::uint64_t>(sizeof(T)) << 32U;
    constexpr std::uint64_t signed_bits = std::is_signed_v<T> ? (1ULL << 24U) : 0ULL;
    constexpr std::uint64_t bool_bits = std::is_same_v<T, bool> ? (1ULL << 25U) : 0ULL;
    return size_bits | signed_bits | bool_bits | 0x464145ULL;
}

} // namespace detail

/**
 * @brief Generate a deterministic probabilistic fingerprint for an integral
 * span.
 *
 * The function is restricted to same-type integral inputs in v1. Empty spans
 * are valid and produce a deterministic empty fingerprint. The produced
 * fingerprint is also tagged by the element type so that fingerprints from
 * different integral types are not intended to compare equal by accident.
 */
template <std::integral T>
[[nodiscard]] Fingerprint
fingerprint(std::span<const T> values, const FingerprintConfig& config = {}) {
    std::vector<std::uint64_t> normalized;
    normalized.reserve(values.size());

    for (const T value : values) {
        normalized.push_back(detail::normalize_integral(value));
    }

    return detail::fingerprint_normalized(
        normalized,
        config.seed,
        detail::integral_type_tag<T>(),
        config.algorithm
    );
}

/**
 * @brief Compare the fingerprints produced for two spans.
 *
 * The result is phrased in terms of the produced fingerprints and is not
 * described as proof of array equality or inequality. The template requires
 * the same element type on both sides.
 */
template <std::integral T>
[[nodiscard]] FingerprintComparison compare_fingerprints(
    std::span<const T> lhs,
    std::span<const T> rhs,
    const FingerprintConfig& config = {}
) {
    return fingerprint(lhs, config) == fingerprint(rhs, config)
               ? FingerprintComparison::fingerprints_match
               : FingerprintComparison::fingerprints_differ;
}

} // namespace fae

#endif

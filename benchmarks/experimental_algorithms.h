#ifndef FAE_BENCHMARKS_EXPERIMENTAL_ALGORITHMS_H
#define FAE_BENCHMARKS_EXPERIMENTAL_ALGORITHMS_H

#include <fae/fingerprint.h>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace fae::benchmarks {

struct LegacyFoldAlgorithm {
    static constexpr std::string_view name = "legacy_fold";

    template <std::integral T>
    [[nodiscard]] static fae::Fingerprint fingerprint(
        std::span<const T> values,
        const fae::FingerprintConfig& config = {}
    ) {
        fae::FingerprintConfig effective_config = config;
        effective_config.algorithm = fae::FingerprintAlgorithm::legacy_fold;
        return fae::fingerprint(values, effective_config);
    }
};

struct HashCombineAlgorithm {
    static constexpr std::string_view name = "hash_combine";

    template <std::integral T>
    [[nodiscard]] static fae::Fingerprint fingerprint(
        std::span<const T> values,
        const fae::FingerprintConfig& config = {}
    ) {
        fae::FingerprintConfig effective_config = config;
        effective_config.algorithm = fae::FingerprintAlgorithm::hash_combine;
        return fae::fingerprint(values, effective_config);
    }
};

struct CantorMixAlgorithm {
    static constexpr std::string_view name = "cantor_mix";

    template <std::integral T>
    [[nodiscard]] static fae::Fingerprint fingerprint(
        std::span<const T> values,
        const fae::FingerprintConfig& config = {}
    ) {
        fae::FingerprintConfig effective_config = config;
        effective_config.algorithm = fae::FingerprintAlgorithm::cantor_mix;
        return fae::fingerprint(values, effective_config);
    }
};

struct TupleMixAlgorithm {
    static constexpr std::string_view name = "tuple_mix";

    template <std::integral T>
    [[nodiscard]] static fae::Fingerprint fingerprint(
        std::span<const T> values,
        const fae::FingerprintConfig& config = {}
    ) {
        fae::FingerprintConfig effective_config = config;
        effective_config.algorithm = fae::FingerprintAlgorithm::tuple_mix;
        return fae::fingerprint(values, effective_config);
    }
};

template <typename Algorithm, std::integral T>
[[nodiscard]] fae::FingerprintComparison compare_fingerprints(
    std::span<const T> lhs,
    std::span<const T> rhs,
    const fae::FingerprintConfig& config = {}
) {
    return Algorithm::fingerprint(lhs, config) == Algorithm::fingerprint(rhs, config)
               ? fae::FingerprintComparison::fingerprints_match
               : fae::FingerprintComparison::fingerprints_differ;
}

} // namespace fae::benchmarks

#endif

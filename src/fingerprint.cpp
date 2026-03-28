#include "fae/fingerprint.h"

#include "fae/reduction/pairwise_fold.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::uint64_t kLaneLowSeed = 0x243F6A8885A308D3ULL;
constexpr std::uint64_t kLaneHighSeed = 0x13198A2E03707344ULL;
constexpr std::uint64_t kLengthTag = 0x9E3779B97F4A7C15ULL;
constexpr std::uint64_t kTypeTagSeed = 0xD1B54A32D192ED03ULL;

[[nodiscard]] constexpr std::uint64_t splitmix64(std::uint64_t value) {
    value += 0x9E3779B97F4A7C15ULL;
    value = (value ^ (value >> 30U)) * 0xBF58476D1CE4E5B9ULL;
    value = (value ^ (value >> 27U)) * 0x94D049BB133111EBULL;
    return value ^ (value >> 31U);
}

[[nodiscard]] constexpr std::uint64_t rotl64(std::uint64_t value, unsigned int amount) {
    return (value << amount) | (value >> (64U - amount));
}

[[nodiscard]] constexpr std::uint64_t cantor_pair_16(
    std::uint16_t lhs,
    std::uint16_t rhs
) {
    const std::uint64_t a = lhs;
    const std::uint64_t b = rhs;
    const std::uint64_t sum = a + b;
    return ((sum * (sum + 1ULL)) / 2ULL) + b;
}

[[nodiscard]] std::uint64_t mix_value(
    std::uint64_t normalized,
    std::size_t index,
    std::uint64_t seed
) {
    const std::uint64_t indexed =
        normalized ^ splitmix64(static_cast<std::uint64_t>(index) + seed);
    return splitmix64(indexed ^ seed);
}

fae::Fingerprint fingerprint_legacy_fold_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag
) {
    std::vector<std::uint64_t> low_lane;
    std::vector<std::uint64_t> high_lane;
    low_lane.reserve(normalized.size());
    high_lane.reserve(normalized.size());

    for (std::size_t index = 0; index < normalized.size(); ++index) {
        low_lane.push_back(mix_value(normalized[index], index, seed ^ kLaneLowSeed));
        high_lane.push_back(mix_value(normalized[index], index, seed ^ kLaneHighSeed));
    }

    const std::uint64_t low =
        splitmix64(
            fae::reduction::pairwise_fold64(low_lane, seed ^ kLaneLowSeed) ^
            static_cast<std::uint64_t>(normalized.size()) ^
            splitmix64(type_tag ^ kTypeTagSeed) ^
            kLengthTag
        );
    const std::uint64_t high =
        splitmix64(
            fae::reduction::pairwise_fold64(high_lane, seed ^ kLaneHighSeed) ^
            (static_cast<std::uint64_t>(normalized.size()) << 1U) ^
            splitmix64(type_tag ^ (kTypeTagSeed << 1U)) ^
            (kLengthTag << 1U)
        );

    return fae::Fingerprint {
        .low = low,
        .high = high,
        .length = normalized.size()
    };
}

fae::Fingerprint fingerprint_hash_combine_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag
) {
    constexpr std::uint64_t kLowSeed = 0x6A09E667F3BCC909ULL;
    constexpr std::uint64_t kHighSeed = 0xBB67AE8584CAA73BULL;
    constexpr std::uint64_t kIndexSeed = 0x3C6EF372FE94F82BULL;

    std::uint64_t low = splitmix64(seed ^ kLowSeed ^ type_tag);
    std::uint64_t high = splitmix64(seed ^ kHighSeed ^ splitmix64(type_tag));

    for (std::size_t index = 0; index < normalized.size(); ++index) {
        const std::uint64_t indexed =
            splitmix64(normalized[index] ^ splitmix64(static_cast<std::uint64_t>(index) ^ kIndexSeed));

        low = splitmix64(low ^ indexed ^ (static_cast<std::uint64_t>(index) << 1U));
        high = splitmix64(
            high + indexed + splitmix64(static_cast<std::uint64_t>(index) ^ seed)
        );
    }

    low = splitmix64(low ^ static_cast<std::uint64_t>(normalized.size()) ^ type_tag);
    high = splitmix64(
        high ^
        (static_cast<std::uint64_t>(normalized.size()) * 0xD6E8FEB86659FD93ULL) ^
        splitmix64(type_tag)
    );

    return fae::Fingerprint {
        .low = low,
        .high = high,
        .length = normalized.size()
    };
}

fae::Fingerprint fingerprint_tuple_mix_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag
) {
    constexpr std::uint64_t kLowSeed = 0xC2B2AE3D27D4EB4FULL;
    constexpr std::uint64_t kHighSeed = 0x165667B19E3779F9ULL;
    constexpr std::uint64_t kPairSeed = 0x85EBCA77C2B2AE63ULL;
    constexpr std::uint64_t kOddFillSeed = 0x27D4EB2F165667C5ULL;

    std::uint64_t low = splitmix64(seed ^ kLowSeed ^ type_tag);
    std::uint64_t high = splitmix64(seed ^ kHighSeed ^ splitmix64(type_tag));

    for (std::size_t index = 0; index < normalized.size(); index += 2U) {
        const std::uint64_t lhs = normalized[index];
        const std::uint64_t rhs =
            index + 1U < normalized.size()
                ? normalized[index + 1U]
                : splitmix64(
                      static_cast<std::uint64_t>(normalized.size()) ^
                      seed ^
                      type_tag ^
                      kOddFillSeed
                  );

        const std::uint64_t pair_index = static_cast<std::uint64_t>(index / 2U);
        const std::uint64_t pair_tag = splitmix64(pair_index ^ seed ^ kPairSeed);
        const std::uint64_t tuple =
            splitmix64(lhs ^ rotl64(rhs, 17U) ^ pair_tag ^ splitmix64(type_tag));

        low = splitmix64(low + tuple + rotl64(tuple, 23U));
        high = splitmix64(high ^ tuple ^ rotl64(tuple, 41U) ^ splitmix64(pair_index));
    }

    low = splitmix64(
        low ^
        static_cast<std::uint64_t>(normalized.size()) ^
        splitmix64(type_tag ^ kLowSeed)
    );
    high = splitmix64(
        high ^
        (static_cast<std::uint64_t>(normalized.size()) * 0x9E3779B185EBCA87ULL) ^
        splitmix64(type_tag ^ kHighSeed)
    );

    return fae::Fingerprint {
        .low = low,
        .high = high,
        .length = normalized.size()
    };
}

fae::Fingerprint fingerprint_cantor_mix_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag
) {
    constexpr std::uint64_t kLowSeed = 0x510E527FADE682D1ULL;
    constexpr std::uint64_t kHighSeed = 0x9B05688C2B3E6C1FULL;
    constexpr std::uint64_t kPairSeed = 0x1F83D9ABFB41BD6BULL;

    std::uint64_t low = splitmix64(seed ^ kLowSeed ^ type_tag);
    std::uint64_t high = splitmix64(seed ^ kHighSeed ^ splitmix64(type_tag));

    for (std::size_t index = 0; index < normalized.size(); ++index) {
        const std::uint64_t value = normalized[index];
        const std::uint16_t p0 = static_cast<std::uint16_t>(value & 0xFFFFULL);
        const std::uint16_t p1 = static_cast<std::uint16_t>((value >> 16U) & 0xFFFFULL);
        const std::uint16_t p2 = static_cast<std::uint16_t>((value >> 32U) & 0xFFFFULL);
        const std::uint16_t p3 = static_cast<std::uint16_t>((value >> 48U) & 0xFFFFULL);

        const std::uint64_t paired_low = cantor_pair_16(p0, p1);
        const std::uint64_t paired_high = cantor_pair_16(p2, p3);
        const std::uint64_t indexed =
            splitmix64(static_cast<std::uint64_t>(index) ^ seed ^ kPairSeed);
        const std::uint64_t combined =
            splitmix64(paired_low ^ splitmix64(paired_high) ^ indexed);

        low = splitmix64(low ^ combined ^ static_cast<std::uint64_t>(index));
        high = splitmix64(high + combined + splitmix64(type_tag ^ indexed));
    }

    low = splitmix64(low ^ static_cast<std::uint64_t>(normalized.size()) ^ type_tag);
    high = splitmix64(
        high ^
        (static_cast<std::uint64_t>(normalized.size()) * 0x94D049BB133111EBULL) ^
        splitmix64(type_tag)
    );

    return fae::Fingerprint {
        .low = low,
        .high = high,
        .length = normalized.size()
    };
}

} // namespace

namespace fae::detail {

Fingerprint
fingerprint_normalized(
    std::span<const std::uint64_t> normalized,
    std::uint64_t seed,
    std::uint64_t type_tag,
    FingerprintAlgorithm algorithm
) {
    switch (algorithm) {
    case FingerprintAlgorithm::legacy_fold:
        return fingerprint_legacy_fold_normalized(normalized, seed, type_tag);
    case FingerprintAlgorithm::hash_combine:
        return fingerprint_hash_combine_normalized(normalized, seed, type_tag);
    case FingerprintAlgorithm::cantor_mix:
        return fingerprint_cantor_mix_normalized(normalized, seed, type_tag);
    case FingerprintAlgorithm::tuple_mix:
        return fingerprint_tuple_mix_normalized(normalized, seed, type_tag);
    }

    return fingerprint_hash_combine_normalized(normalized, seed, type_tag);
}

} // namespace fae::detail

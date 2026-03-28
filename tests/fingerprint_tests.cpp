#include <fae/fingerprint.h>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <span>
#include <vector>

namespace {

TEST(FingerprintTest, ProducesStableFingerprintsForSameInput) {
    const std::vector<std::int32_t> values {1, 2, 3, 4, 5};

    const fae::Fingerprint first = fae::fingerprint(std::span(values));
    const fae::Fingerprint second = fae::fingerprint(std::span(values));

    EXPECT_EQ(first, second);
}

TEST(FingerprintTest, SupportsEmptyInput) {
    const std::vector<std::uint16_t> values;
    const fae::Fingerprint result = fae::fingerprint(std::span(values));

    EXPECT_EQ(result.length, 0U);
    EXPECT_EQ(result, fae::fingerprint(std::span(values)));
}

TEST(FingerprintTest, FingerprintIsOrderSensitive) {
    const std::vector<std::int32_t> lhs {1, 2, 3, 4};
    const std::vector<std::int32_t> rhs {4, 3, 2, 1};

    EXPECT_NE(fae::fingerprint(std::span(lhs)), fae::fingerprint(std::span(rhs)));
}

TEST(FingerprintTest, FingerprintIncludesLength) {
    const std::vector<std::uint32_t> lhs {7, 8, 9};
    const std::vector<std::uint32_t> rhs {7, 8, 9, 0};

    EXPECT_NE(fae::fingerprint(std::span(lhs)), fae::fingerprint(std::span(rhs)));
}

TEST(FingerprintTest, CompareReportsMatchingFingerprintsForEqualInputs) {
    const std::vector<std::int64_t> lhs {9, 8, 7, 6};
    const std::vector<std::int64_t> rhs {9, 8, 7, 6};

    EXPECT_EQ(
        fae::compare_fingerprints(std::span(lhs), std::span(rhs)),
        fae::FingerprintComparison::fingerprints_match
    );
}

TEST(FingerprintTest, CompareReportsDifferingFingerprintsForDifferentInputs) {
    const std::vector<std::uint32_t> lhs {1, 2, 3, 4};
    const std::vector<std::uint32_t> rhs {1, 2, 3, 5};

    EXPECT_EQ(
        fae::compare_fingerprints(std::span(lhs), std::span(rhs)),
        fae::FingerprintComparison::fingerprints_differ
    );
}

TEST(FingerprintTest, SeedChangesFingerprintOutput) {
    const std::vector<std::int8_t> values {1, 2, 3, 4};
    const fae::FingerprintConfig first_config {.seed = 1U};
    const fae::FingerprintConfig second_config {.seed = 2U};

    EXPECT_NE(
        fae::fingerprint(std::span(values), first_config),
        fae::fingerprint(std::span(values), second_config)
    );
}

TEST(FingerprintTest, AlgorithmSelectionChangesFingerprintOutput) {
    const std::vector<std::uint32_t> values {1, 2, 3, 4, 5, 6};
    const fae::FingerprintConfig legacy_config {
        .seed = 7U,
        .algorithm = fae::FingerprintAlgorithm::legacy_fold
    };
    const fae::FingerprintConfig hash_combine_config {
        .seed = 7U,
        .algorithm = fae::FingerprintAlgorithm::hash_combine
    };
    const fae::FingerprintConfig cantor_mix_config {
        .seed = 7U,
        .algorithm = fae::FingerprintAlgorithm::cantor_mix
    };
    const fae::FingerprintConfig tuple_mix_config {
        .seed = 7U,
        .algorithm = fae::FingerprintAlgorithm::tuple_mix
    };

    EXPECT_NE(
        fae::fingerprint(std::span(values), legacy_config),
        fae::fingerprint(std::span(values), hash_combine_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), hash_combine_config),
        fae::fingerprint(std::span(values), cantor_mix_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), hash_combine_config),
        fae::fingerprint(std::span(values), tuple_mix_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), cantor_mix_config),
        fae::fingerprint(std::span(values), tuple_mix_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), legacy_config),
        fae::fingerprint(std::span(values), tuple_mix_config)
    );
}

TEST(FingerprintTest, DefaultConfigurationUsesHashCombine) {
    const std::vector<std::uint32_t> values {1U, 2U, 3U, 4U, 5U, 6U};
    const fae::FingerprintConfig default_config {.seed = 11U};
    const fae::FingerprintConfig hash_combine_config {
        .seed = 11U,
        .algorithm = fae::FingerprintAlgorithm::hash_combine
    };
    const fae::FingerprintConfig legacy_config {
        .seed = 11U,
        .algorithm = fae::FingerprintAlgorithm::legacy_fold
    };
    const fae::FingerprintConfig cantor_mix_config {
        .seed = 11U,
        .algorithm = fae::FingerprintAlgorithm::cantor_mix
    };
    const fae::FingerprintConfig tuple_mix_config {
        .seed = 11U,
        .algorithm = fae::FingerprintAlgorithm::tuple_mix
    };

    EXPECT_EQ(
        fae::fingerprint(std::span(values), default_config),
        fae::fingerprint(std::span(values), hash_combine_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), default_config),
        fae::fingerprint(std::span(values), legacy_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), default_config),
        fae::fingerprint(std::span(values), cantor_mix_config)
    );
    EXPECT_NE(
        fae::fingerprint(std::span(values), default_config),
        fae::fingerprint(std::span(values), tuple_mix_config)
    );
}

TEST(FingerprintTest, WorksAcrossIntegralTypesOnSameTypeBasis) {
    const std::vector<unsigned char> values {1U, 2U, 3U, 4U};
    const fae::Fingerprint result = fae::fingerprint(std::span(values));

    EXPECT_EQ(result.length, values.size());
}

TEST(FingerprintTest, FingerprintIsTaggedByElementType) {
    const std::vector<std::int32_t> signed_values {1, 2, 3, 4};
    const std::vector<std::uint32_t> unsigned_values {1U, 2U, 3U, 4U};

    EXPECT_NE(
        fae::fingerprint(std::span(signed_values)),
        fae::fingerprint(std::span(unsigned_values))
    );
}

TEST(FingerprintTest, SupportsBooleanInputs) {
    constexpr std::array<bool, 4> lhs {true, false, true, true};
    constexpr std::array<bool, 4> rhs {true, false, true, false};

    EXPECT_EQ(
        fae::compare_fingerprints(std::span<const bool>(lhs), std::span<const bool>(lhs)),
        fae::FingerprintComparison::fingerprints_match
    );
    EXPECT_EQ(
        fae::compare_fingerprints(std::span<const bool>(lhs), std::span<const bool>(rhs)),
        fae::FingerprintComparison::fingerprints_differ
    );
}

TEST(FingerprintTest, CompareUsesSelectedAlgorithm) {
    const std::vector<std::int32_t> lhs {10, 20, 30, 40};
    const std::vector<std::int32_t> rhs {10, 20, 30, 41};
    const fae::FingerprintConfig config {
        .seed = 17U,
        .algorithm = fae::FingerprintAlgorithm::hash_combine
    };

    EXPECT_EQ(
        fae::compare_fingerprints(std::span(lhs), std::span(lhs), config),
        fae::FingerprintComparison::fingerprints_match
    );
    EXPECT_EQ(
        fae::compare_fingerprints(std::span(lhs), std::span(rhs), config),
        fae::FingerprintComparison::fingerprints_differ
    );
}

TEST(FingerprintTest, PublicTupleMixAlgorithmIsStableAndSelectable) {
    const std::vector<std::int32_t> values {3, 1, 4, 1, 5, 9, 2};
    const fae::FingerprintConfig tuple_mix_config {
        .seed = 29U,
        .algorithm = fae::FingerprintAlgorithm::tuple_mix
    };
    const fae::FingerprintConfig hash_combine_config {
        .seed = 29U,
        .algorithm = fae::FingerprintAlgorithm::hash_combine
    };

    const fae::Fingerprint first = fae::fingerprint(std::span(values), tuple_mix_config);
    const fae::Fingerprint second = fae::fingerprint(std::span(values), tuple_mix_config);

    EXPECT_EQ(first, second);
    EXPECT_NE(first, fae::fingerprint(std::span(values), hash_combine_config));
}

TEST(FingerprintTest, PublicCantorMixAlgorithmIsStableAndSelectable) {
    const std::vector<std::uint32_t> values {8U, 6U, 7U, 5U, 3U, 0U, 9U};
    const fae::FingerprintConfig cantor_mix_config {
        .seed = 31U,
        .algorithm = fae::FingerprintAlgorithm::cantor_mix
    };
    const fae::FingerprintConfig hash_combine_config {
        .seed = 31U,
        .algorithm = fae::FingerprintAlgorithm::hash_combine
    };

    const fae::Fingerprint first = fae::fingerprint(std::span(values), cantor_mix_config);
    const fae::Fingerprint second = fae::fingerprint(std::span(values), cantor_mix_config);

    EXPECT_EQ(first, second);
    EXPECT_NE(first, fae::fingerprint(std::span(values), hash_combine_config));
}

} // namespace

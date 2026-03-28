#include <fae/fae.h>

#include "experimental_algorithms.h"

#include <array>
#include <charconv>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace {

enum class CollisionProfile {
    smoke,
    standard,
    deep
};

struct CollisionConfig {
    CollisionProfile profile {CollisionProfile::standard};
    std::uint64_t seed {0xF00D1234ULL};
};

struct FingerprintHash {
    std::size_t operator()(const fae::Fingerprint& fingerprint) const noexcept {
        std::size_t seed = static_cast<std::size_t>(fingerprint.low);
        seed ^= static_cast<std::size_t>(fingerprint.high + 0x9E3779B97F4A7C15ULL + (seed << 6U) + (seed >> 2U));
        seed ^= static_cast<std::size_t>(fingerprint.length + 0x85EBCA77C2B2AE63ULL + (seed << 6U) + (seed >> 2U));
        return seed;
    }
};

enum class DistributionKind {
    uniform_full_range,
    low_range,
    sparse_boolean,
    monotonic_step,
    paired_low_range
};

struct CollisionStats {
    std::size_t samples {};
    std::size_t unique_fingerprints {};
    std::size_t duplicate_inputs {};
    std::size_t true_collisions {};
};

template <typename T>
T sample_low_range_value(std::mt19937_64& rng) {
    if constexpr (std::is_signed_v<T>) {
        std::uniform_int_distribution<int> dist(-16, 16);
        return static_cast<T>(dist(rng));
    } else {
        std::uniform_int_distribution<unsigned int> dist(0U, 32U);
        return static_cast<T>(dist(rng));
    }
}

template <typename T>
std::vector<T> make_sample(
    std::size_t length,
    DistributionKind distribution,
    std::mt19937_64& rng
) {
    std::vector<T> values(length);

    switch (distribution) {
    case DistributionKind::uniform_full_range: {
        if constexpr (std::is_signed_v<T>) {
            std::uniform_int_distribution<long long> dist(
                static_cast<long long>(std::numeric_limits<T>::min()),
                static_cast<long long>(std::numeric_limits<T>::max())
            );
            for (T& value : values) {
                value = static_cast<T>(dist(rng));
            }
        } else {
            std::uniform_int_distribution<unsigned long long> dist(
                static_cast<unsigned long long>(std::numeric_limits<T>::min()),
                static_cast<unsigned long long>(std::numeric_limits<T>::max())
            );
            for (T& value : values) {
                value = static_cast<T>(dist(rng));
            }
        }
        break;
    }
    case DistributionKind::low_range: {
        for (T& value : values) {
            value = sample_low_range_value<T>(rng);
        }
        break;
    }
    case DistributionKind::sparse_boolean: {
        std::bernoulli_distribution dist(0.1);
        for (T& value : values) {
            value = dist(rng) ? static_cast<T>(1) : static_cast<T>(0);
        }
        break;
    }
    case DistributionKind::monotonic_step: {
        const std::int64_t base = static_cast<std::int64_t>(sample_low_range_value<T>(rng));
        std::uniform_int_distribution<int> step_dist(-3, 3);
        int step = step_dist(rng);
        if (step == 0) {
            step = 1;
        }
        std::uniform_int_distribution<int> jitter_dist(-1, 1);

        for (std::size_t index = 0; index < values.size(); ++index) {
            const std::int64_t jitter = static_cast<std::int64_t>(jitter_dist(rng));
            const std::int64_t candidate =
                base + (static_cast<std::int64_t>(index) * static_cast<std::int64_t>(step)) + jitter;
            values[index] = static_cast<T>(candidate);
        }
        break;
    }
    case DistributionKind::paired_low_range: {
        std::bernoulli_distribution tweak_dist(0.35);

        for (std::size_t index = 0; index < values.size(); index += 2U) {
            const T base = sample_low_range_value<T>(rng);
            values[index] = base;
            if (index + 1U < values.size()) {
                if (tweak_dist(rng)) {
                    values[index + 1U] = static_cast<T>(
                        static_cast<std::int64_t>(base) + 1LL
                    );
                } else {
                    values[index + 1U] = base;
                }
            }
        }
        break;
    }
    }

    return values;
}

template <typename T>
using CollisionBucket = std::unordered_map<
    fae::Fingerprint,
    std::vector<std::vector<T>>,
    FingerprintHash
>;

template <typename Algorithm, typename T>
CollisionStats run_collision_study(
    std::size_t samples,
    std::size_t length,
    DistributionKind distribution,
    std::uint64_t seed
) {
    std::mt19937_64 rng(seed);
    CollisionBucket<T> seen;
    seen.reserve(samples);

    CollisionStats stats {.samples = samples};
    const fae::FingerprintConfig config {.seed = seed};

    for (std::size_t index = 0; index < samples; ++index) {
        std::vector<T> values = make_sample<T>(length, distribution, rng);
        const fae::Fingerprint fingerprint =
            Algorithm::fingerprint(std::span<const T>(values), config);
        auto& bucket = seen[fingerprint];

        bool duplicate = false;
        for (const auto& existing : bucket) {
            if (existing == values) {
                duplicate = true;
                ++stats.duplicate_inputs;
                break;
            }
        }

        if (!duplicate && !bucket.empty()) {
            ++stats.true_collisions;
        }

        bucket.push_back(std::move(values));
    }

    stats.unique_fingerprints = seen.size();
    return stats;
}

std::string_view distribution_name(DistributionKind distribution) {
    switch (distribution) {
    case DistributionKind::uniform_full_range:
        return "uniform_full_range";
    case DistributionKind::low_range:
        return "low_range";
    case DistributionKind::sparse_boolean:
        return "sparse_boolean";
    case DistributionKind::monotonic_step:
        return "monotonic_step";
    case DistributionKind::paired_low_range:
        return "paired_low_range";
    }
    return "unknown";
}

std::array<std::size_t, 3> lengths_for_profile(CollisionProfile profile) {
    switch (profile) {
    case CollisionProfile::smoke:
        return {4U, 8U, 16U};
    case CollisionProfile::standard:
        return {4U, 16U, 64U};
    case CollisionProfile::deep:
        return {16U, 64U, 256U};
    }

    return {4U, 16U, 64U};
}

std::size_t samples_for_profile(CollisionProfile profile, std::size_t length) {
    switch (profile) {
    case CollisionProfile::smoke:
        return length <= 8U ? 512U : 256U;
    case CollisionProfile::standard:
        return length <= 16U ? 10000U : 5000U;
    case CollisionProfile::deep:
        return length <= 64U ? 50000U : 20000U;
    }

    return length <= 16U ? 10000U : 5000U;
}

std::string_view profile_name(CollisionProfile profile) {
    switch (profile) {
    case CollisionProfile::smoke:
        return "smoke";
    case CollisionProfile::standard:
        return "default";
    case CollisionProfile::deep:
        return "deep";
    }

    return "default";
}

bool parse_profile(std::string_view text, CollisionProfile& profile) {
    if (text == "smoke") {
        profile = CollisionProfile::smoke;
        return true;
    }

    if (text == "default") {
        profile = CollisionProfile::standard;
        return true;
    }

    if (text == "deep") {
        profile = CollisionProfile::deep;
        return true;
    }

    return false;
}

bool parse_seed(std::string_view text, std::uint64_t& seed) {
    seed = 0;
    const char* begin = text.data();
    const char* end = text.data() + text.size();
    const auto result = std::from_chars(begin, end, seed);
    return result.ec == std::errc {} && result.ptr == end;
}

void print_help() {
    std::cout << "Usage: fae_collision_study [--profile=smoke|default|deep] [--seed=<u64>]\n";
    std::cout << "  smoke   reduced sample counts for CI and quick checks\n";
    std::cout << "  default current local baseline profile\n";
    std::cout << "  deep    larger empirical campaign for local investigation\n";
}

bool parse_arguments(int argc, char** argv, CollisionConfig& config) {
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument(argv[index]);

        if (argument == "--help" || argument == "-h") {
            print_help();
            return false;
        }

        constexpr std::string_view profile_prefix = "--profile=";
        if (argument.starts_with(profile_prefix)) {
            if (!parse_profile(argument.substr(profile_prefix.size()), config.profile)) {
                std::cerr << "Invalid collision profile: " << argument << '\n';
                return false;
            }
            continue;
        }

        constexpr std::string_view seed_prefix = "--seed=";
        if (argument.starts_with(seed_prefix)) {
            if (!parse_seed(argument.substr(seed_prefix.size()), config.seed)) {
                std::cerr << "Invalid seed: " << argument << '\n';
                return false;
            }
            continue;
        }

        std::cerr << "Unrecognized argument: " << argument << '\n';
        return false;
    }

    return true;
}

template <typename T>
void print_stats(
    std::string_view algorithm_name,
    std::string_view type_name,
    std::size_t length,
    DistributionKind distribution,
    const CollisionStats& stats
) {
    const double collision_rate = stats.samples == 0
                                      ? 0.0
                                      : static_cast<double>(stats.true_collisions) /
                                            static_cast<double>(stats.samples);

    std::cout << std::left << std::setw(16) << algorithm_name
              << std::setw(10) << type_name
              << std::setw(10) << length
              << std::setw(20) << distribution_name(distribution)
              << std::setw(12) << stats.samples
              << std::setw(12) << stats.unique_fingerprints
              << std::setw(12) << stats.duplicate_inputs
              << std::setw(12) << stats.true_collisions
              << std::fixed << std::setprecision(6) << collision_rate << '\n';
}

template <typename Algorithm, typename T>
void run_suite_for_algorithm(
    std::string_view type_name,
    const CollisionConfig& config,
    std::uint64_t seed
) {
    constexpr std::array<DistributionKind, 5> distributions {
        DistributionKind::uniform_full_range,
        DistributionKind::low_range,
        DistributionKind::sparse_boolean,
        DistributionKind::monotonic_step,
        DistributionKind::paired_low_range
    };

    for (const std::size_t length : lengths_for_profile(config.profile)) {
        const std::size_t samples = samples_for_profile(config.profile, length);
        for (const DistributionKind distribution : distributions) {
            print_stats<T>(
                Algorithm::name,
                type_name,
                length,
                distribution,
                run_collision_study<Algorithm, T>(samples, length, distribution, seed)
            );
        }
    }
}

template <typename T>
void run_suite(std::string_view type_name, const CollisionConfig& config, std::uint64_t seed) {
    run_suite_for_algorithm<fae::benchmarks::LegacyFoldAlgorithm, T>(type_name, config, seed);
    run_suite_for_algorithm<fae::benchmarks::HashCombineAlgorithm, T>(type_name, config, seed);
    run_suite_for_algorithm<fae::benchmarks::CantorMixAlgorithm, T>(type_name, config, seed);
    run_suite_for_algorithm<fae::benchmarks::TupleMixAlgorithm, T>(type_name, config, seed);
}

} // namespace

int main(int argc, char** argv) {
    CollisionConfig config;
    if (!parse_arguments(argc, argv, config)) {
        return 0;
    }

    std::cout << "FAE collision study\n";
    std::cout << "Empirical results depend on sample size and input distribution.\n\n";
    std::cout << "Profile: " << profile_name(config.profile) << ", seed: " << config.seed
              << "\n\n";
    std::cout << std::left << std::setw(16) << "algorithm"
              << std::setw(10) << "type"
              << std::setw(10) << "length"
              << std::setw(20) << "distribution"
              << std::setw(12) << "samples"
              << std::setw(12) << "unique_fp"
              << std::setw(12) << "duplicates"
              << std::setw(12) << "collisions"
              << "collision_rate" << '\n';

    run_suite<std::int32_t>("int32_t", config, config.seed);
    run_suite<std::uint64_t>("uint64_t", config, config.seed ^ 0x55AAULL);
    run_suite<std::uint8_t>("uint8_t", config, config.seed ^ 0xAA55ULL);

    return 0;
}

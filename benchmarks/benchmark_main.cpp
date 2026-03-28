#include <fae/fae.h>

#include "experimental_algorithms.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <span>
#include <string_view>
#include <type_traits>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

enum class BenchmarkProfile {
    smoke,
    standard,
    deep
};

struct BenchmarkConfig {
    BenchmarkProfile profile {BenchmarkProfile::standard};
};

struct BenchmarkRow {
    std::string_view algorithm;
    std::string_view name;
    std::size_t size;
    std::size_t iterations;
    double nanoseconds_per_call;
    double mib_per_second;
};

template <typename T>
std::vector<T> make_sequence(std::size_t size, std::uint64_t seed) {
    std::vector<T> values(size);
    std::uint64_t state = seed ^ 0x9E3779B97F4A7C15ULL;

    for (std::size_t index = 0; index < size; ++index) {
        state ^= state >> 12U;
        state ^= state << 25U;
        state ^= state >> 27U;
        const std::uint64_t mixed = state * 2685821657736338717ULL;

        values[index] = static_cast<T>(mixed);
    }

    return values;
}

template <typename Callable>
double measure_nanoseconds_per_call(Callable&& callable, std::size_t iterations) {
    const auto start = Clock::now();
    for (std::size_t index = 0; index < iterations; ++index) {
        callable();
    }
    const auto finish = Clock::now();
    const auto elapsed =
        std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
    return static_cast<double>(elapsed) / static_cast<double>(iterations);
}

template <typename Algorithm, typename T>
BenchmarkRow benchmark_fingerprint_case(std::size_t size, std::size_t iterations) {
    const std::vector<T> values = make_sequence<T>(size, 0x12345678ULL);
    const std::span<const T> span(values);

    volatile std::uint64_t sink = 0;
    const double nanoseconds = measure_nanoseconds_per_call(
        [&]() {
            const fae::Fingerprint fingerprint = Algorithm::fingerprint(span);
            sink ^= fingerprint.low;
            sink ^= fingerprint.high;
        },
        iterations
    );
    static_cast<void>(sink);

    const double mib_per_second =
        (static_cast<double>(size * sizeof(T)) / (1024.0 * 1024.0)) /
        (nanoseconds / 1'000'000'000.0);

    return BenchmarkRow {
        .algorithm = Algorithm::name,
        .name = "fingerprint",
        .size = size,
        .iterations = iterations,
        .nanoseconds_per_call = nanoseconds,
        .mib_per_second = mib_per_second
    };
}

template <typename Algorithm, typename T>
BenchmarkRow benchmark_compare_case(
    std::size_t size,
    std::size_t iterations,
    bool identical_inputs
) {
    const std::vector<T> lhs = make_sequence<T>(size, 0xABCDEFULL);
    std::vector<T> rhs = lhs;
    if (!identical_inputs && !rhs.empty()) {
        rhs.back() = static_cast<T>(rhs.back() ^ static_cast<T>(1));
    }

    const std::span<const T> lhs_span(lhs);
    const std::span<const T> rhs_span(rhs);

    volatile std::uint64_t sink = 0;
    const double nanoseconds = measure_nanoseconds_per_call(
        [&]() {
            const auto comparison =
                fae::benchmarks::compare_fingerprints<Algorithm>(lhs_span, rhs_span);
            sink ^= comparison == fae::FingerprintComparison::fingerprints_match ? 1ULL : 0ULL;
        },
        iterations
    );
    static_cast<void>(sink);

    const double mib_per_second =
        (static_cast<double>((size * sizeof(T)) * 2U) / (1024.0 * 1024.0)) /
        (nanoseconds / 1'000'000'000.0);

    return BenchmarkRow {
        .algorithm = Algorithm::name,
        .name = identical_inputs ? "compare_match" : "compare_differ",
        .size = size,
        .iterations = iterations,
        .nanoseconds_per_call = nanoseconds,
        .mib_per_second = mib_per_second
    };
}

void print_header(std::string_view type_name) {
    std::cout << "\n[" << type_name << "]\n";
    std::cout << std::left << std::setw(16) << "algorithm"
              << std::setw(18) << "benchmark"
              << std::right << std::setw(12) << "size"
              << std::setw(14) << "iters"
              << std::setw(16) << "ns/op"
              << std::setw(16) << "MiB/s" << '\n';
}

void print_row(const BenchmarkRow& row) {
    std::cout << std::left << std::setw(16) << row.algorithm
              << std::setw(18) << row.name
              << std::right << std::setw(12) << row.size
              << std::setw(14) << row.iterations
              << std::setw(16) << std::fixed << std::setprecision(2)
              << row.nanoseconds_per_call
              << std::setw(16) << std::fixed << std::setprecision(2)
              << row.mib_per_second << '\n';
}

std::size_t target_bytes_for_profile(BenchmarkProfile profile) {
    switch (profile) {
    case BenchmarkProfile::smoke:
        return 64U * 1024U;
    case BenchmarkProfile::standard:
        return 16U * 1024U * 1024U;
    case BenchmarkProfile::deep:
        return 64U * 1024U * 1024U;
    }

    return 16U * 1024U * 1024U;
}

std::size_t max_iterations_for_profile(BenchmarkProfile profile) {
    switch (profile) {
    case BenchmarkProfile::smoke:
        return 256U;
    case BenchmarkProfile::standard:
        return 8192U;
    case BenchmarkProfile::deep:
        return 16384U;
    }

    return 8192U;
}

std::size_t min_iterations_for_profile(BenchmarkProfile profile) {
    switch (profile) {
    case BenchmarkProfile::smoke:
        return 8U;
    case BenchmarkProfile::standard:
        return 64U;
    case BenchmarkProfile::deep:
        return 64U;
    }

    return 64U;
}

std::size_t choose_iterations(
    std::size_t size,
    std::size_t element_size,
    BenchmarkProfile profile
) {
    const std::size_t target_bytes = target_bytes_for_profile(profile);
    const std::size_t bytes_per_call = std::max<std::size_t>(1U, size * element_size);
    return std::clamp<std::size_t>(
        target_bytes / bytes_per_call,
        min_iterations_for_profile(profile),
        max_iterations_for_profile(profile)
    );
}

std::array<std::size_t, 4> sizes_for_profile(BenchmarkProfile profile) {
    switch (profile) {
    case BenchmarkProfile::smoke:
        return {8U, 64U, 512U, 4096U};
    case BenchmarkProfile::standard:
        return {16U, 256U, 4096U, 65536U};
    case BenchmarkProfile::deep:
        return {64U, 1024U, 16384U, 262144U};
    }

    return {16U, 256U, 4096U, 65536U};
}

std::string_view profile_name(BenchmarkProfile profile) {
    switch (profile) {
    case BenchmarkProfile::smoke:
        return "smoke";
    case BenchmarkProfile::standard:
        return "default";
    case BenchmarkProfile::deep:
        return "deep";
    }

    return "default";
}

bool parse_profile(std::string_view text, BenchmarkProfile& profile) {
    if (text == "smoke") {
        profile = BenchmarkProfile::smoke;
        return true;
    }

    if (text == "default") {
        profile = BenchmarkProfile::standard;
        return true;
    }

    if (text == "deep") {
        profile = BenchmarkProfile::deep;
        return true;
    }

    return false;
}

void print_help() {
    std::cout << "Usage: fae_benchmark [--profile=smoke|default|deep]\n";
    std::cout << "  smoke   reduced sizes and iterations for CI and quick checks\n";
    std::cout << "  default current local baseline profile\n";
    std::cout << "  deep    larger inputs and more work for local investigation\n";
}

bool parse_arguments(int argc, char** argv, BenchmarkConfig& config) {
    for (int index = 1; index < argc; ++index) {
        const std::string_view argument(argv[index]);

        if (argument == "--help" || argument == "-h") {
            print_help();
            return false;
        }

        constexpr std::string_view profile_prefix = "--profile=";
        if (argument.starts_with(profile_prefix)) {
            if (!parse_profile(argument.substr(profile_prefix.size()), config.profile)) {
                std::cerr << "Invalid benchmark profile: " << argument << '\n';
                return false;
            }
            continue;
        }

        std::cerr << "Unrecognized argument: " << argument << '\n';
        return false;
    }

    return true;
}

template <typename Algorithm, typename T>
void run_suite_for_algorithm(const BenchmarkConfig& config) {
    for (const std::size_t size : sizes_for_profile(config.profile)) {
        const std::size_t iterations =
            choose_iterations(size, sizeof(T), config.profile);
        print_row(benchmark_fingerprint_case<Algorithm, T>(size, iterations));
        print_row(benchmark_compare_case<Algorithm, T>(size, iterations, true));
        print_row(benchmark_compare_case<Algorithm, T>(size, iterations, false));
    }
}

template <typename T>
void run_suite(std::string_view type_name, const BenchmarkConfig& config) {
    print_header(type_name);
    run_suite_for_algorithm<fae::benchmarks::LegacyFoldAlgorithm, T>(config);
    run_suite_for_algorithm<fae::benchmarks::HashCombineAlgorithm, T>(config);
    run_suite_for_algorithm<fae::benchmarks::CantorMixAlgorithm, T>(config);
    run_suite_for_algorithm<fae::benchmarks::TupleMixAlgorithm, T>(config);
}

} // namespace

int main(int argc, char** argv) {
    BenchmarkConfig config;
    if (!parse_arguments(argc, argv, config)) {
        return 0;
    }

    std::cout << std::unitbuf;
    std::cout << "FAE throughput benchmark\n";
    std::cout << "Measurements are simple wall-clock estimates intended for trend analysis.\n";
    std::cout << "Profile: " << profile_name(config.profile) << "\n";

    run_suite<std::int32_t>("int32_t", config);
    run_suite<std::uint64_t>("uint64_t", config);
    run_suite<std::uint8_t>("uint8_t", config);

    return 0;
}

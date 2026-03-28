#include "fae/reduction/pairwise_fold.h"

#include <cstdint>
#include <vector>

namespace {

constexpr std::uint64_t kEmptyTag = 0xA0761D6478BD642FULL;
constexpr std::uint64_t kTailTag = 0xE7037ED1A0B428DBULL;

[[nodiscard]] constexpr std::uint64_t splitmix64(std::uint64_t value) {
    value += 0x9E3779B97F4A7C15ULL;
    value = (value ^ (value >> 30U)) * 0xBF58476D1CE4E5B9ULL;
    value = (value ^ (value >> 27U)) * 0x94D049BB133111EBULL;
    return value ^ (value >> 31U);
}

[[nodiscard]] constexpr std::uint64_t mix_pair(
    std::uint64_t lhs,
    std::uint64_t rhs,
    std::uint64_t seed
) {
    return splitmix64(lhs ^ splitmix64(rhs + seed));
}

} // namespace

namespace fae::reduction {

std::uint64_t
pairwise_fold64(std::span<const std::uint64_t> values, std::uint64_t seed) {
    if (values.empty()) {
        return splitmix64(seed ^ kEmptyTag);
    }

    std::vector<std::uint64_t> current(values.begin(), values.end());
    std::vector<std::uint64_t> next;
    next.reserve((current.size() + 1U) / 2U);

    std::uint64_t round_seed = seed;
    while (current.size() > 1U) {
        next.clear();

        for (std::size_t index = 0; index + 1U < current.size(); index += 2U) {
            next.push_back(mix_pair(current[index], current[index + 1U], round_seed));
        }

        if ((current.size() % 2U) != 0U) {
            next.push_back(
                mix_pair(current.back(), round_seed ^ kTailTag, round_seed ^ kTailTag)
            );
        }

        current.swap(next);
        round_seed = splitmix64(round_seed ^ static_cast<std::uint64_t>(current.size()));
    }

    return splitmix64(current.front() ^ round_seed);
}

} // namespace fae::reduction

#include <fae/reduction/pairwise_fold.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

namespace {

TEST(PairwiseFoldTest, HandlesEmptyInputDeterministically) {
    EXPECT_EQ(fae::reduction::pairwise_fold64({}, 7U), fae::reduction::pairwise_fold64({}, 7U));
    EXPECT_NE(fae::reduction::pairwise_fold64({}, 7U), fae::reduction::pairwise_fold64({}, 8U));
}

TEST(PairwiseFoldTest, HandlesSingleValueInput) {
    const std::vector<std::uint64_t> values {42U};
    EXPECT_EQ(
        fae::reduction::pairwise_fold64(values, 11U),
        fae::reduction::pairwise_fold64(values, 11U)
    );
}

TEST(PairwiseFoldTest, DistinguishesOddAndEvenLayouts) {
    const std::vector<std::uint64_t> odd {1U, 2U, 3U};
    const std::vector<std::uint64_t> even {1U, 2U, 3U, 4U};

    EXPECT_NE(
        fae::reduction::pairwise_fold64(odd, 5U),
        fae::reduction::pairwise_fold64(even, 5U)
    );
}

TEST(PairwiseFoldTest, IsOrderSensitive) {
    const std::vector<std::uint64_t> lhs {1U, 2U, 3U, 4U};
    const std::vector<std::uint64_t> rhs {4U, 3U, 2U, 1U};

    EXPECT_NE(
        fae::reduction::pairwise_fold64(lhs, 0U),
        fae::reduction::pairwise_fold64(rhs, 0U)
    );
}

} // namespace

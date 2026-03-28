#include <fae/pairing/szudzik.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>

namespace {

TEST(SzudzikPairingTest, EncodesKnownValues) {
    EXPECT_EQ(fae::pairing::szudzik_pair(1, 2), 18U);
    EXPECT_EQ(fae::pairing::szudzik_pair(5, 10), 410U);
    EXPECT_EQ(fae::pairing::szudzik_pair(-3, 4), 69U);
}

TEST(SzudzikPairingTest, PreservesOrdering) {
    EXPECT_NE(fae::pairing::szudzik_pair(1, 2), fae::pairing::szudzik_pair(2, 1));
    EXPECT_NE(
        fae::pairing::szudzik_pair(-6, 5),
        fae::pairing::szudzik_pair(5, -6)
    );
}

TEST(SzudzikPairingTest, HandlesInt32Extremes) {
    EXPECT_EQ(
        fae::pairing::szudzik_pair(
            std::numeric_limits<std::int32_t>::max(),
            std::numeric_limits<std::int32_t>::max()
        ),
        18446744065119617024ULL
    );
    EXPECT_EQ(
        fae::pairing::szudzik_pair(
            std::numeric_limits<std::int32_t>::min(),
            std::numeric_limits<std::int32_t>::min()
        ),
        18446744073709551615ULL
    );
}

} // namespace

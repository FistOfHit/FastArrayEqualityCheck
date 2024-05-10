#include "szudzik.h"
#include <gtest/gtest.h>

/**
 * @brief Test case for the Szudzik pairing function with positive input values.
 */
TEST(SzudzikPairingTest, PositiveNumbers) {
    EXPECT_EQ(szudzikPairing(1, 2), 18);
    EXPECT_EQ(szudzikPairing(5, 10), 410);
    EXPECT_EQ(szudzikPairing(6, 55), 12112);
}

/**
 * @brief Test case for the Szudzik pairing function with negative input values.
 */
TEST(SzudzikPairingTest, NegativeNumbers) {
    EXPECT_EQ(szudzikPairing(-3, 4), 69);
    EXPECT_EQ(szudzikPairing(5, -6), 131);
    EXPECT_EQ(szudzikPairing(-7, -8), 238);
}

/**
 * @brief Test case for the Szudzik pairing function with zero input values.
 */
TEST(SzudzikPairingTest, ZeroValues) {
    EXPECT_EQ(szudzikPairing(0, 0), 0);
    EXPECT_EQ(szudzikPairing(0, 5), 100);
    EXPECT_EQ(szudzikPairing(-3, 0), 30);
}

/**
 * @brief Test case for the Szudzik pairing function with input values in the opposite order.
 */
TEST(SzudzikPairingTest, OppositeOrder) {
    EXPECT_NE(szudzikPairing(1, 2), szudzikPairing(2, 1));
    EXPECT_NE(szudzikPairing(-3, 4), szudzikPairing(4, -3));
    EXPECT_NE(szudzikPairing(5, -6), szudzikPairing(-6, 5));
}

/**
 * @brief Test case for the Szudzik pairing function with input values at the int32 limits.
 */
TEST(SzudzikPairingTest, Int32Limits) {
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()), 18446744065119617024ULL);
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()), 18446744073709551615ULL);
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min()), 18446744069414584319ULL);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

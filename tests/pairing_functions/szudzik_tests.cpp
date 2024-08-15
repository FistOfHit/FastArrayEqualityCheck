/**
 * @file szudzik_tests.cpp
 * @brief Unit tests for the Szudzik pairing function.
 *
 * This file contains unit tests for the Szudzik pairing function
 * implementation. It uses the Google Test framework to test various
 * scenarios including positive numbers, negative numbers, zero values,
 * and edge cases with int32 limits.
 */

#include "pairing_functions/szudzik.h"
#include <gtest/gtest.h>
#include <iostream>
#include <iomanip>

class SzudzikPairingTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << std::endl << "----------------------------------------" << std::endl;
    }

    void TearDown() override {
        std::cout << "----------------------------------------" << std::endl;
    }

    void PrintTestCase(const std::string& description, int32_t a, int32_t b, uint64_t expected) {
        std::cout << std::left << description 
                  << "(" <<  std::right << a << ", " 
                  << std::right << b << ") = " 
                  << std::right << expected << std::endl;
    }
};

TEST_F(SzudzikPairingTest, PositiveNumbers) {
    std::cout << "Testing positive numbers:" << std::endl;
    PrintTestCase("Pairing", 1, 2, 18);
    EXPECT_EQ(szudzikPairing(1, 2), 18);
    
    PrintTestCase("Pairing", 5, 10, 410);
    EXPECT_EQ(szudzikPairing(5, 10), 410);
    
    PrintTestCase("Pairing", 6, 55, 12112);
    EXPECT_EQ(szudzikPairing(6, 55), 12112);
}

TEST_F(SzudzikPairingTest, NegativeNumbers) {
    std::cout << "Testing negative numbers:" << std::endl;
    PrintTestCase("Pairing", -3, 4, 69);
    EXPECT_EQ(szudzikPairing(-3, 4), 69);
    
    PrintTestCase("Pairing", 5, -6, 131);
    EXPECT_EQ(szudzikPairing(5, -6), 131);
    
    PrintTestCase("Pairing", -7, -8, 238);
    EXPECT_EQ(szudzikPairing(-7, -8), 238);
}

TEST_F(SzudzikPairingTest, ZeroValues) {
    std::cout << "Testing zero values:" << std::endl;
    PrintTestCase("Pairing", 0, 0, 0);
    EXPECT_EQ(szudzikPairing(0, 0), 0);
    
    PrintTestCase("Pairing", 0, 5, 100);
    EXPECT_EQ(szudzikPairing(0, 5), 100);
    
    PrintTestCase("Pairing", -3, 0, 30);
    EXPECT_EQ(szudzikPairing(-3, 0), 30);
}

TEST_F(SzudzikPairingTest, OppositeOrder) {
    std::cout << "Testing opposite order:" << std::endl;
    PrintTestCase("Pairing", 1, 2, szudzikPairing(1, 2));
    PrintTestCase("Pairing", 2, 1, szudzikPairing(2, 1));
    EXPECT_NE(szudzikPairing(1, 2), szudzikPairing(2, 1));
    
    PrintTestCase("Pairing", -3, 4, szudzikPairing(-3, 4));
    PrintTestCase("Pairing", 4, -3, szudzikPairing(4, -3));
    EXPECT_NE(szudzikPairing(-3, 4), szudzikPairing(4, -3));
    
    PrintTestCase("Pairing", 5, -6, szudzikPairing(5, -6));
    PrintTestCase("Pairing", -6, 5, szudzikPairing(-6, 5));
    EXPECT_NE(szudzikPairing(5, -6), szudzikPairing(-6, 5));
}

TEST_F(SzudzikPairingTest, Int32Limits) {
    std::cout << "Testing int32 limits:" << std::endl;
    PrintTestCase("Pairing", std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max(), 18446744065119617024ULL);
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max()), 18446744065119617024ULL);
    
    PrintTestCase("Pairing", std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min(), 18446744073709551615ULL);
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::min()), 18446744073709551615ULL);
    
    PrintTestCase("Pairing", std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min(), 18446744069414584319ULL);
    EXPECT_EQ(szudzikPairing(std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::min()), 18446744069414584319ULL);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

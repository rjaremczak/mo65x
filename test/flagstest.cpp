#include <gtest/gtest.h>
#include "processorstatus.h"

class FlagsTest : public ::testing::Test
{
protected:
  ProcessorStatus p;
};

TEST(Fa,Fu) {
  EXPECT_EQ(true,false);
}

TEST_F(FlagsTest, testBits) {
  p = 0b11111111;
  EXPECT_EQ(p.toByte(), 0b11001111);
}

TEST_F(FlagsTest, testNegative) {
  p.computeN(0b10000000);
  EXPECT_EQ(p.negative, true);
  p.computeN(0b01111111);
  EXPECT_EQ(p.negative, false);
  int8_t num = -2;
  p.computeN(static_cast<uint8_t>(num));
  EXPECT_EQ(p.negative, true);
}

TEST_F(FlagsTest, testZero) {
  p.computeZ(0);
  EXPECT_EQ(p.zero, true);
  p.computeZ(0x80);
  EXPECT_EQ(p.zero, false);
  p.computeZ(1);
  EXPECT_EQ(p.zero, false);
}

TEST_F(FlagsTest, testCarry) {
  p.computeC(0b100000000);
  EXPECT_EQ(p.carry, true);
  p.computeC(0b010000000);
  EXPECT_EQ(p.carry, false);
  p.computeC(0b011111111);
  EXPECT_EQ(p.carry, false);
  p.computeC(130);
  EXPECT_EQ(p.carry, false);
  p.computeC(300);
  EXPECT_EQ(p.carry, true);
}

TEST_F(FlagsTest, testOverflow) {
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(10), 130);
  EXPECT_EQ(p.overflow, true);
  p.computeV(200, 100, static_cast<uint8_t>(300));
  EXPECT_EQ(p.overflow, false);
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(7), 127);
  EXPECT_EQ(p.overflow, false);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(-10), static_cast<uint8_t>(-130));
  EXPECT_EQ(p.overflow, true);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(7), 127);
  EXPECT_EQ(p.overflow, false);
}

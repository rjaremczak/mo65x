#include <gtest/gtest.h>
#include "flagstest.h"
#include "instructionstest.h"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


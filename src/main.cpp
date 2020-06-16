#include "main-imgui-sfml.h"
#include "commondefs.h"
#include "config.h"
#include "emulatorstate.h"
#include "filedatastorage.h"
#include "test/assemblertest.h"
#include "test/flagstest.h"
#include "test/instructionstest.h"
#include <gtest/gtest.h>

int gtest(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

int main(int, char**) {
#ifdef RUN_TESTS
  return gtest(argc, argv);
#else
  main_imgui_sfml();
  return 0;
#endif
}

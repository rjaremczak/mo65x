#include "assemblertest.h"
#include "flagstest.h"
#include "instructionstest.h"
#include <QtTest>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  AssemblerTest assemblerTest;
  OpCodesTest opCodesTest;
  FlagsTest flagsTest;

  return QTest::qExec(&opCodesTest, argc, argv) | QTest::qExec(&assemblerTest, argc, argv) | QTest::qExec(&flagsTest, argc, argv);
}

#include "assemblertest.h"
#include "flagstest.h"
#include "instructionstest.h"
#include <QTest>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  AssemblerTest assemblerTest;
  InstructionsTest opCodesTest;
  FlagsTest flagsTest;

  return QTest::qExec(&opCodesTest, argc, argv) | QTest::qExec(&assemblerTest, argc, argv) | QTest::qExec(&flagsTest, argc, argv);
}

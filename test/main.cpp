#include "assemblertest.h"
#include "opcodestest.h"
#include <QtTest>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  AssemblerTest assemblerTest;
  OpCodesTest opCodesTest;

  return QTest::qExec(&opCodesTest, argc, argv) | QTest::qExec(&assemblerTest, argc, argv);
}

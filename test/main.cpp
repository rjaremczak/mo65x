#include "opcodestest.h"
#include <QtTest>

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  OpCodesTest opCodesTest;

  return QTest::qExec(&opCodesTest, argc, argv);
}

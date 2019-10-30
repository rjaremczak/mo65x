#include "opcodestest.h"
#include <QTest>

OpCodesTest::OpCodesTest(QObject* parent) : QObject(parent), cpu_(memory_) {
}

void OpCodesTest::initTestCase() {
  QVERIFY(&cpu_.memory_ == &memory_);
}

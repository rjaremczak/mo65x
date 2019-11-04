#include "flagstest.h"
#include <QTest>

FlagsTest::FlagsTest(QObject* parent) : QObject(parent) {
}

void FlagsTest::init() {
}

void FlagsTest::testNegative() {
  p.computeN(0b10000000);
  QCOMPARE(p.n, true);
  p.computeN(0b01111111);
  QCOMPARE(p.n, false);
  int8_t num = -2;
  p.computeN(static_cast<uint8_t>(num));
  QCOMPARE(p.n, true);
}

void FlagsTest::testZero() {
  p.computeZ(0);
  QCOMPARE(p.z, true);
  p.computeZ(0x80);
  QCOMPARE(p.z, false);
  p.computeZ(1);
  QCOMPARE(p.z, false);
}

void FlagsTest::testCarry() {
  p.computeC(0b100000000);
  QCOMPARE(p.c, true);
  p.computeC(0b010000000);
  QCOMPARE(p.c, false);
  p.computeC(0b011111111);
  QCOMPARE(p.c, false);
  p.computeC(130);
  QCOMPARE(p.c, false);
  p.computeC(300);
  QCOMPARE(p.c, true);
}

void FlagsTest::testOverflow() {
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(10), 130);
  QCOMPARE(p.v, true);
  p.computeV(200, 100, static_cast<uint8_t>(300));
  QCOMPARE(p.v, false);
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(7), 127);
  QCOMPARE(p.v, false);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(-10), static_cast<uint8_t>(-130));
  QCOMPARE(p.v, true);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(7), 127);
  QCOMPARE(p.v, false);
}

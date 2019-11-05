#include "flagstest.h"
#include <QTest>

FlagsTest::FlagsTest(QObject* parent) : QObject(parent) {
}

void FlagsTest::init() {
}

void FlagsTest::testNegative() {
  p.computeN(0b10000000);
  QCOMPARE(p.negative, true);
  p.computeN(0b01111111);
  QCOMPARE(p.negative, false);
  int8_t num = -2;
  p.computeN(static_cast<uint8_t>(num));
  QCOMPARE(p.negative, true);
}

void FlagsTest::testZero() {
  p.computeZ(0);
  QCOMPARE(p.zero, true);
  p.computeZ(0x80);
  QCOMPARE(p.zero, false);
  p.computeZ(1);
  QCOMPARE(p.zero, false);
}

void FlagsTest::testCarry() {
  p.computeC(0b100000000);
  QCOMPARE(p.carry, true);
  p.computeC(0b010000000);
  QCOMPARE(p.carry, false);
  p.computeC(0b011111111);
  QCOMPARE(p.carry, false);
  p.computeC(130);
  QCOMPARE(p.carry, false);
  p.computeC(300);
  QCOMPARE(p.carry, true);
}

void FlagsTest::testOverflow() {
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(10), 130);
  QCOMPARE(p.overflow, true);
  p.computeV(200, 100, static_cast<uint8_t>(300));
  QCOMPARE(p.overflow, false);
  p.computeV(static_cast<int8_t>(120), static_cast<int8_t>(7), 127);
  QCOMPARE(p.overflow, false);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(-10), static_cast<uint8_t>(-130));
  QCOMPARE(p.overflow, true);
  p.computeV(static_cast<uint8_t>(-120), static_cast<uint8_t>(7), 127);
  QCOMPARE(p.overflow, false);
}

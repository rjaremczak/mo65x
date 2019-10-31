#include "assemblertest.h"
#include <QTest>

AssemblerTest::AssemblerTest(QObject* parent) : QObject(parent), assembler(memory) {
}

void AssemblerTest::init() {
  assembler.setOrigin(AsmOrigin);
}

void AssemblerTest::testImpliedModeFromStr() {
  assembler.enter("SEI");
  QCOMPARE(memory[AsmOrigin], 0x78);
}

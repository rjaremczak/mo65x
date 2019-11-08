#include "disassembler.h"
#include "instruction.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QStringList>
#include <map>

static QString formatHex8(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

static QString formatHex16(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}

QString Disassembler::formatOperand8() const {
  return "$" + formatHex8(memory[address + 1]);
}

QString Disassembler::formatOperand16() const {
  return "$" + formatHex16(memory.read16(address + 1));
}

Disassembler::Disassembler(const Memory& memory, uint16_t pc) : memory(memory) {
  setOrigin(pc);
}

void Disassembler::setOrigin(uint16_t addr) {
  address = addr;
  opcode = memory[addr];
  instruction = InstructionTable[opcode];
}

void Disassembler::nextInstruction() {
  setOrigin(address + instruction.size);
}

QString Disassembler::dumpBytes(uint16_t n) const {
  QString str;
  for (uint16_t i = 0; i < n; i++) {
    str.append(formatHex8(memory[address + i])).append(" ");
  }
  return str;
}

QString Disassembler::dumpWords(uint16_t n) const {
  QString str;
  for (uint16_t i = 0; i < n; i += 2) {
    str.append(formatHex16(memory.read16(address + i))).append(" ");
  }
  return str;
}

QString Disassembler::disassemble() const {
  QString str = formatHex16(address).append(" ");
  for (uint8_t i = 0; i < 3; i++) {
    str.append(i < instruction.size ? formatHex8(memory[address + i]).append(" ") : "   ");
  }
  str.append(" ");
  str.append(MnemonicTable.at(instruction.type)).append(" ");

  switch (instruction.mode) {
  case NoOperands:
  case Implied:
  case Accumulator: break;
  case Immediate: str.append("#").append(formatOperand8()); break;
  case Absolute: str.append(formatOperand16()); break;
  case AbsoluteX: str.append(formatOperand16()).append(",X"); break;
  case AbsoluteY: str.append(formatOperand16()).append(",Y"); break;
  case ZeroPage: str.append(formatOperand8()); break;
  case ZeroPageX: str.append(formatOperand8()).append(",X"); break;
  case ZeroPageY: str.append(formatOperand8()).append(",Y"); break;
  case IndexedIndirectX: str.append("(").append(formatOperand8()).append(",X)"); break;
  case IndirectIndexedY: str.append("(").append(formatOperand8()).append("),Y"); break;
  case Indirect: str.append("(").append(formatOperand16()).append(")"); break;
  case Relative:
    const auto displacement = static_cast<int8_t>(memory[address + 1]);
    if (displacement > 0) str.append("+");
    str.append(QString::number(displacement));
    break;
  }
  return str.toUpper();
}

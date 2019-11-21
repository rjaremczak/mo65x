#include "disassembler.h"
#include "commondefs.h"
#include "formatters.h"
#include "instruction.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QStringList>
#include <map>

QString Disassembler::formatOperand8() const {
  return "$" + formatHexByte(memory[address + 1]);
}

QString Disassembler::formatOperand16() const {
  return "$" + formatHexWord(memory.word(address + 1));
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
  for (uint16_t i = 0; i < n; i++) { str.append(formatHexByte(memory[address + i])).append(" "); }
  return str;
}

QString Disassembler::dumpWords(uint16_t n) const {
  QString str;
  for (uint16_t i = 0; i < n; i += 2) { str.append(formatHexWord(memory.word(address + i))).append(" "); }
  return str;
}

QString Disassembler::disassemble() const {
  QString str;
  for (uint8_t i = 0; i < 3; i++) { str.append(i < instruction.size ? formatHexByte(memory[address + i]).append(" ") : "   "); }
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
  case Branch:
    const auto displacement = static_cast<int8_t>(memory[address + 1]);
    if (displacement > 0) str.append("+");
    str.append(QString::number(displacement));
    break;
  }
  return str.toUpper();
}

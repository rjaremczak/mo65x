#include "disassembler.h"
#include "instruction.h"
#include <QStringList>
#include <map>

static const std::map<InstructionType, const char*> Mnemonics{
    {ADC, "ADC"}, {SBC, "SBC"}, {AND, "AND"}, {ORA, "ORA"}, {ASL, "ASL"}, {LSR, "LSR"}, {EOR, "EOR"}, {ROL, "ROL"},
    {ROR, "ROR"}, {BIT, "BIT"}, {CMP, "CMP"}, {CPX, "CPX"}, {CPY, "CPY"}, {INC, "INC"}, {INX, "INX"}, {INY, "INY"},
    {DEC, "DEC"}, {DEX, "DEX"}, {DEY, "DEY"}, {BCC, "BCC"}, {BCS, "BCS"}, {BEQ, "BEQ"}, {BMI, "BMI"}, {BNE, "BNE"},
    {BPL, "BPL"}, {BVC, "BVC"}, {BVS, "BVS"}, {CLC, "CLC"}, {CLD, "CLD"}, {CLI, "CLI"}, {CLV, "CLV"}, {SEC, "SEC"},
    {SED, "SED"}, {SEI, "SEI"}, {JMP, "JMP"}, {JSR, "JSR"}, {BRK, "BRK"}, {RTI, "RTI"}, {RTS, "RTS"}, {LDA, "LDA"},
    {LDX, "LDX"}, {LDY, "LDY"}, {STA, "STA"}, {STX, "STX"}, {STY, "STY"}, {TAX, "TAX"}, {TAY, "TAY"}, {TSX, "TSX"},
    {TXA, "TXA"}, {TYA, "TYA"}, {TXS, "TXS"}, {PHA, "PHA"}, {PHP, "PHP"}, {PLA, "PLA"}, {PLP, "PLP"}, {NOP, "NOP"},
    {INV, "???"}};

static QString formatHex8(uint8_t val) {
  return QString("%1").arg(val, 2, 16, QChar('0'));
}

static QString formatHex16(uint16_t val) {
  return QString("%1").arg(val, 4, 16, QChar('0'));
}

QString Disassembler::formatOperand8() const {
  return "$" + formatHex8(memory_[address_ + 1]);
}

QString Disassembler::formatOperand16() const {
  return "$" + formatHex16(memory_.read16(address_ + 1));
}

Disassembler::Disassembler(const Memory& memory, uint16_t pc) : memory_(memory) {
  setAddress(pc);
}

void Disassembler::setAddress(uint16_t addr) {
  address_ = addr;
  opcode_ = memory_[addr];
  instruction_ = OpCodeTable[opcode_];
}

void Disassembler::step() {
  setAddress(address_ + instruction_.size);
}

QString Disassembler::dumpBytes(uint16_t n) const {
  QString str;
  for (uint16_t i = 0; i < n; i++) {
    str.append(formatHex8(memory_[address_ + i])).append(" ");
  }
  return str;
}

QString Disassembler::dumpWords(uint16_t n) const {
  QString str;
  for (uint16_t i = 0; i < n; i += 2) {
    str.append(formatHex16(memory_.read16(address_ + i))).append(" ");
  }
  return str;
}

QString Disassembler::disassemble() const {
  QString str = formatHex16(address_).append(" ");
  for (uint8_t i = 0; i < 3; i++) {
    str.append(i < instruction_.size ? formatHex8(memory_[address_ + i]).append(" ") : "   ");
  }
  str.append(" ");
  str.append(Mnemonics.at(instruction_.instruction)).append(" ");

  switch (instruction_.addressing) {
  case Implied:
  case Accumulator:
    break;
  case Immediate:
    str.append("#").append(formatOperand8());
    break;
  case Absolute:
    str.append(formatOperand16());
    break;
  case AbsoluteX:
    str.append(formatOperand16()).append(",X");
    break;
  case AbsoluteY:
    str.append(formatOperand16()).append(",Y");
    break;
  case ZeroPage:
    str.append(formatOperand8());
    break;
  case ZeroPageX:
    str.append(formatOperand8()).append(",X");
    break;
  case ZeroPageY:
    str.append(formatOperand8()).append(",Y");
    break;
  case IndexedIndirectX:
    str.append("(").append(formatOperand8()).append(",X)");
    break;
  case IndirectIndexedY:
    str.append("(").append(formatOperand8()).append("),Y");
    break;
  case Indirect:
    str.append("(").append(formatOperand16()).append(")");
    break;
  case Relative:
    const auto displacement = static_cast<int8_t>(memory_[address_ + 1]);
    str.append("$").append(formatHex16(static_cast<uint16_t>(address_ + instruction_.size + displacement)));
    break;
  }
  return str.toUpper();
}

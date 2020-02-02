#include "disassembler.h"
#include "commondefs.h"
#include "commonformatters.h"
#include "instruction.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include <QStringList>
#include <map>

QString Disassembler::formatOperand8() const {
  return "$" + formatHexByte(m_memory[m_address + 1]);
}

QString Disassembler::formatOperand16() const {
  return "$" + formatHexWord(m_memory.word(m_address + 1));
}

Disassembler::Disassembler(const Memory& memory, Address pc) : m_memory(memory) {
  setOrigin(pc);
}

void Disassembler::setOrigin(Address addr) {
  m_address = addr;
  m_opcode = m_memory[addr];
  m_instruction = InstructionTable[m_opcode];
}

void Disassembler::nextInstruction() {
  setOrigin(m_address + m_instruction.size);
}

QString Disassembler::disassemble() const {
  QString str;
  for (uint8_t i = 0; i < 3; i++) {
    str.append(i < m_instruction.size ? formatHexByte(m_memory[m_address + i]).append(" ") : "   ");
  }
  str.append(" ");
  str.append(MnemonicTable.at(m_instruction.type)).append(" ");

  switch (m_instruction.mode) {
  case ImpliedOrAccumulator: break;
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
    const auto displacement = static_cast<int8_t>(m_memory[m_address + 1]);
    if (displacement > 0) str.append("+");
    str.append(QString::number(displacement));
    break;
  }
  return str.toUpper();
}

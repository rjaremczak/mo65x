#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include "utilities.h"
#include <QRegularExpression>
#include <algorithm>

static const QRegularExpression ImpliedPattern("(\\w{3})\\s*");
static const QRegularExpression ImmediatePattern("(\\w{3}\\s+#($?)(\\d|[a-h|A-H])");

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

void Assembler::enter(InstructionType type, AddressingMode amode, uint16_t operand) {
  const auto it = find(type, amode);
  if (it == InstructionTable.end()) {
    valid_ = false;
    return;
  }

  memory_[address_++] = static_cast<uint8_t>(std::distance(InstructionTable.begin(), it));
  if (it->size == 2) {
    memory_[address_++] = loByte(operand);
  } else if (it->size == 3) {
    memory_.write16(address_ += 2, operand);
  }
  valid_ = true;
}

void Assembler::enter(const char* str) {
  QString qs(str);
  QRegularExpressionMatch match = ImpliedPattern.match(qs);
  if (match.hasMatch()) {
    QString mnemonic = match.captured(1);
    if (auto type = findInstructionTypeForMnemonic(mnemonic); type != INV) enter(type);
  }
}

const Instruction* Assembler::find(InstructionType type, AddressingMode amode) const {
  return std::find_if(InstructionTable.begin(), InstructionTable.end(),
                      [=](const Instruction& ins) { return ins.type == type && ins.addressingMode == amode; });
}

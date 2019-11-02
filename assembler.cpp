#include "assembler.h"
#include "instructiontable.h"
#include "mnemonics.h"
#include "utilities.h"
#include <algorithm>
#include <regex>

struct AddressingModeEntry {
  std::regex pattern;
  AddressingMode mode;
};

struct AddressingModeInference {
  std::smatch match;
  AddressingMode mode;
};

const AddressingModeEntry AddressingModeEntries[]{
    {std::regex(R"(([A-Z]{3})$)"), NoOperands},
    {std::regex(R"(([A-Z]{3})\s+#\$([\d|A-H]{2})$)"), Immediate},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{2})$)"), ZeroPage},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{2})\s*,\s*X$)"), ZeroPageX},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{2})\s*,\s*Y$)"), ZeroPageY},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})$)"), Absolute},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})\s*,\s*X$)"), AbsoluteX},
    {std::regex(R"(([A-Z]{3})\s+\$([\d|A-H]{3,4})\s*,\s*Y$)"), AbsoluteY},
    {std::regex(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,4})\)\s*$)"), Indirect},
    {std::regex(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,2}),X\)$)"), IndexedIndirectX},
    {std::regex(R"(([A-Z]{3})\s+\(\$([\d|A-H]{1,2})\),Y$)"), IndirectIndexedY},
    {std::regex(R"(([A-Z]{3})\s+([+|-]?\d{1,3})$)"), Relative}};

static const Instruction* findInstruction(InstructionType type, AddressingMode mode) {
  if (type == INV) return nullptr;

  const auto mode0 = mode == NoOperands ? Implied : mode;
  const auto mode1 = mode == NoOperands ? Accumulator : mode;
  const auto it = std::find_if(InstructionTable.begin(), InstructionTable.end(), [=](const Instruction& ins) {
    return ins.type == type && (ins.mode == mode0 || ins.mode == mode1);
  });
  return it != InstructionTable.end() ? it : nullptr;
}

static AddressingModeInference inferAddressingMode(std::string str) {
  std::transform(str.begin(), str.end(), str.begin(), [](auto c) { return std::toupper(c); });
  AddressingModeInference inf;
  for (auto& entry : AddressingModeEntries) {
    if (std::regex_match(str, inf.match, entry.pattern)) {
      inf.mode = entry.mode;
      break;
    }
  }
  return inf;
}

Assembler::Assembler(Memory& memory, uint16_t origin) : memory_(memory), address_(origin) {
}

bool Assembler::assemble(InstructionType type, AddressingMode mode, int operand) {
  if (const auto it = findInstruction(type, mode)) {
    memory_[address_++] = static_cast<uint8_t>(std::distance(InstructionTable.begin(), it));
    if (it->size > 1) memory_[address_++] = loByte(operand);
    if (it->size > 2) memory_[address_++] = hiByte(operand);
    return true;
  }
  return false;
}

bool Assembler::assemble(const char* str) {
  const auto inf = inferAddressingMode(str);
  if (inf.match.empty()) return false;

  const auto type = findInstructionType(inf.match[1].str());
  if (inf.mode == Relative) {
    return assemble(type, inf.mode, std::stoi(inf.match[2], nullptr, 10));
  } else if (Instruction::sizeForAddressingMode(inf.mode) > 1) {
    return assemble(type, inf.mode, std::stoi(inf.match[2], nullptr, 16));
  } else {
    return assemble(type);
  }
}

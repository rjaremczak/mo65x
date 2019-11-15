#pragma once

#include "instructiontype.h"
#include "operandsformat.h"
#include <cstdint>

struct Instruction {
  static constexpr auto NumberOfOpCodes = 256;
  static constexpr uint8_t sizeForAddressingMode(OperandsFormat mode) {
    switch (mode) {
    case Implied:
    case Accumulator:
    case NoOperands: return 1;

    case Branch:
    case Immediate:
    case ZeroPage:
    case ZeroPageX:
    case ZeroPageY:
    case IndexedIndirectX:
    case IndirectIndexedY: return 2;

    case Indirect:
    case Absolute:
    case AbsoluteX:
    case AbsoluteY: return 3;
    }
    return 0;
  }

  InstructionType type = INV;
  OperandsFormat mode = Implied;
  uint8_t size = 1;
  uint8_t cycles = 0;

  Instruction() = default;

  constexpr Instruction(InstructionType type, OperandsFormat mode, uint8_t cycles) {
    this->type = type;
    this->mode = mode;
    this->cycles = cycles;
    this->size = sizeForAddressingMode(mode);
  }
};

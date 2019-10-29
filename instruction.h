#pragma once

#include <array>
#include "instructiontype.h"
#include "addressingmode.h"

struct Instruction {
  static constexpr auto NumberOfOpCodes = 256;

  InstructionType type = INV;
  AddressingMode addressingMode = Implied;
  uint8_t size = 1;
  uint8_t cycles = 0;

  Instruction() = default;

  constexpr Instruction(InstructionType type, AddressingMode addressingMode, uint8_t cycles) {
    this->type = type;
    this->addressingMode = addressingMode;
    this->cycles = cycles;
    this->size = [addressingMode]() -> uint8_t {
      switch (addressingMode) {
      case Implied:
      case Accumulator:
        return 1;
      case Relative:
      case Immediate:
      case ZeroPage:
      case ZeroPageX:
      case ZeroPageY:
      case IndexedIndirectX:
      case IndirectIndexedY:
        return 2;
      case Indirect:
      case Absolute:
      case AbsoluteX:
      case AbsoluteY:
        return 3;
      }
    }();
  }
};

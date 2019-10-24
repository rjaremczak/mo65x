#pragma once

#include <array>
#include "instructiontype.h"
#include "addressingmode.h"

struct Instruction
{
    InstructionType instruction = Invalid;
    AddressingMode addressing = Implied;
    uint8_t size = 1;
    uint8_t cycles = 0;

    Instruction() = default;
    Instruction(InstructionType instruction, AddressingMode addressing, uint8_t cycles);
};

extern const std::array<Instruction, 256> OpCodeTable;

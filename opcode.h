#pragma once

#include <array>
#include "instruction.h"
#include "addressingmode.h"

struct Operation
{
    Instruction instruction = Invalid;
    AddressingMode addressing = Implied;
    uint8_t size = 1;
    uint8_t cycles = 0;

    Operation() = default;
    Operation(Instruction instruction, AddressingMode addressing, uint8_t cycles);
};

extern const std::array<Operation, 256> OpCodeTable;

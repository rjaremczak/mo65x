#pragma once

#include <array>
#include "instruction.h"
#include "addressingmode.h"

struct OpCode
{
    Instruction instruction = Invalid;
    AddressingMode addressing = Implied;
    uint8_t size = 1;
    uint8_t cycles = 0;

    OpCode() = default;
    OpCode(Instruction instruction, AddressingMode addressing, uint8_t cycles);
};

using OpCodesArray = std::array<OpCode, 256>;

extern const OpCodesArray OpCodes;

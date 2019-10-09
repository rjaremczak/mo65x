#pragma once

#include <cstdint>
#include "addressingmode.h"

struct Instruction
{
    const char* mnemonic;
    const AddressingMode addressingMode;
    const uint8_t bytes;
    const uint8_t cycles;

    inline bool valid() const { return mnemonic; }

};

static constexpr Instruction InvalidInstruction { nullptr, Unknown, 0, 0 };

extern const Instruction Instructions[256];

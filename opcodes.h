#pragma once

#include <array>
#include "instruction.h"

using OpCodesArray = std::array<Instruction, 256>;

extern const OpCodesArray OpCodes;

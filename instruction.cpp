#include "instruction.h"

const Instruction Instructions[256] {

    { "BRK", Implied, 1, 7 },
    { "ORA", IndexedXIndirect, 2, 6 },
    InvalidInstruction,
    InvalidInstruction,
    InvalidInstruction,
    { "ORA", ZeroPage, 2, 3 },
    { "ASL", ZeroPage, 2, 5 },
    InvalidInstruction,
    { "PHP", Implied, 1, 3 },
    { "ORA", Immediate, 2, 2 },
    { "ASL", Accumulator, 1, 2 },
    InvalidInstruction,
    InvalidInstruction,
    { "ORA", Absolute, 3, 4 },
    { "ASL", Absolute, 3, 6 },
    InvalidInstruction,

    { "BPL", Relative, 2, 2 },
    { "ORA", IndirectIndexedY, 2, 5 },
    InvalidInstruction,
    InvalidInstruction,
    InvalidInstruction,
    { "ORA"}

};

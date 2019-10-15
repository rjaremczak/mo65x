#include "instruction.h"

Instruction::Instruction(Mnemonic mnemonic, Addressing addressing, uint8_t cycles)
{
    this->mnemonic = mnemonic;
    this->addressing = addressing;
    this->cycles = cycles;
    this->size = [addressing]() -> uint8_t {
        switch (addressing) {
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

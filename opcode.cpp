#include "opcode.h"

Operation::Operation(Instruction mnemonic, AddressingMode addressing, uint8_t cycles)
{
    this->instruction = mnemonic;
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

const std::array<Operation, 256> OpCodeTable = []{
    std::array<Operation, 256> arr;
    arr.fill({ Invalid, Implied, 0 });
    arr[0x00] = { BRK, Implied, 7 };
    arr[0x01] = { ORA, IndexedIndirectX, 6 };
    arr[0x05] = { ORA, ZeroPage, 3 };
    arr[0x06] = { ASL, ZeroPage, 5 };
    arr[0x08] = { PHP, Implied, 3 };
    arr[0x09] = { ORA, Immediate, 2 };
    arr[0x0a] = { ASL, Accumulator, 2 };
    arr[0x0d] = { ORA, Absolute, 4 };
    arr[0x0e] = { ASL, Absolute, 6 };

    arr[0x10] = { BPL, Relative, 2 };
    arr[0x11] = { ORA, IndirectIndexedY, 5 };
    arr[0x15] = { ORA, ZeroPageX, 4 };
    arr[0x16] = { ASL, ZeroPageX, 6 };
    arr[0x18] = { CLC, Implied, 2 };
    arr[0x19] = { ORA, AbsoluteY, 4 };
    arr[0x1d] = { ORA, AbsoluteX, 4 };
    arr[0x1e] = { ASL, AbsoluteX, 7 };

    arr[0x20] = { JSR, Absolute, 6 };
    arr[0x21] = { AND, IndexedIndirectX, 6 };
    arr[0x24] = { BIT, ZeroPage, 3 };
    arr[0x25] = { AND, ZeroPage, 3 };
    arr[0x26] = { ROL, ZeroPage, 5 };
    arr[0x28] = { PLP, Implied, 4 };
    arr[0x29] = { AND, Immediate, 2 };
    arr[0x2a] = { ROL, Accumulator, 2 };
    arr[0x2c] = { BIT, Absolute, 4 };
    arr[0x2d] = { AND, Absolute, 4 };
    arr[0x2e] = { ROL, Absolute, 6 };

    arr[0x30] = { BMI, Relative, 2 };
    arr[0x31] = { AND, IndirectIndexedY, 5 };
    arr[0x35] = { AND, ZeroPageX, 4 };
    arr[0x36] = { ROL, ZeroPageX, 6 };
    arr[0x38] = { SEC, Implied, 2 };
    arr[0x39] = { AND, AbsoluteY, 4 };
    arr[0x3d] = { AND, AbsoluteX, 4 };
    arr[0x3e] = { ROL, AbsoluteX, 7 };

    arr[0x40] = { RTI, Implied, 6 };
    arr[0x41] = { EOR, IndexedIndirectX, 6 };
    arr[0x45] = { EOR, ZeroPage, 3 };
    arr[0x46] = { LSR, ZeroPage, 5 };
    arr[0x48] = { PHA, Implied, 3 };
    arr[0x49] = { EOR, Immediate, 2 };
    arr[0x4a] = { LSR, Accumulator, 2 };
    arr[0x4c] = { JMP, Absolute, 3 };
    arr[0x4d] = { EOR, Absolute, 4 };
    arr[0x4e] = { LSR, Absolute, 6 };

    arr[0x50] = { BVC, Relative, 2 };
    arr[0x51] = { EOR, IndirectIndexedY, 5 };
    arr[0x55] = { EOR, ZeroPageX, 4 };
    arr[0x56] = { LSR, ZeroPageX, 6 };
    arr[0x58] = { CLI, Implied, 2 };
    arr[0x59] = { EOR, AbsoluteY, 4 };
    arr[0x5d] = { EOR, AbsoluteX, 4 };
    arr[0x5e] = { LSR, AbsoluteX, 7 };

    arr[0x60] = { RTS, Implied, 6 };
    arr[0x61] = { ADC, IndexedIndirectX, 6 };
    arr[0x65] = { ADC, ZeroPage, 3 };
    arr[0x66] = { ROR, ZeroPage, 5 };
    arr[0x68] = { PLA, Implied, 4 };
    arr[0x69] = { ADC, Immediate, 2 };
    arr[0x6a] = { ROR, Accumulator, 2 };
    arr[0x6c] = { JMP, Indirect, 6 };
    arr[0x6d] = { ADC, Absolute, 4 };
    arr[0x6e] = { ROR, Absolute, 6 };

    arr[0x70] = { BVS, Relative, 2 };
    arr[0x71] = { ADC, IndirectIndexedY, 5 };
    arr[0x75] = { ADC, ZeroPageX, 4 };
    arr[0x76] = { ROR, ZeroPageX, 6 };
    arr[0x78] = { SEI, Implied, 2 };
    arr[0x79] = { ADC, AbsoluteY, 4 };
    arr[0x7d] = { ADC, AbsoluteX, 4 };
    arr[0x7e] = { ROR, AbsoluteX, 7 };

    arr[0x81] = { STA, IndexedIndirectX, 6 };
    arr[0x84] = { STY, ZeroPage, 3 };
    arr[0x85] = { STA, ZeroPage, 3 };
    arr[0x86] = { STX, ZeroPage, 3 };
    arr[0x88] = { DEY, Implied, 2 };
    arr[0x8a] = { TXA, Implied, 2 };
    arr[0x8c] = { STY, Absolute, 4 };
    arr[0x8d] = { STA, Absolute, 4 };
    arr[0x8e] = { STX, Absolute, 4 };

    arr[0x90] = { BCC, Relative, 2 };
    arr[0x91] = { STA, IndirectIndexedY, 6 };
    arr[0x94] = { STY, ZeroPageX, 4 };
    arr[0x95] = { STA, ZeroPageX, 4 };
    arr[0x96] = { STX, ZeroPageY, 4 };
    arr[0x98] = { TYA, Implied, 2 };
    arr[0x99] = { STA, AbsoluteY, 5 };
    arr[0x9a] = { TXS, Implied, 2 };
    arr[0x9d] = { STA, AbsoluteX, 5 };

    arr[0xa0] = { LDY, Immediate, 2 };
    arr[0xa1] = { LDA, IndexedIndirectX, 6 };
    arr[0xa2] = { LDX, Immediate, 2 };
    arr[0xa4] = { LDY, ZeroPage, 3 };
    arr[0xa5] = { LDA, ZeroPage, 3 };
    arr[0xa6] = { LDX, ZeroPage, 3 };
    arr[0xa8] = { TAY, Implied, 2 };
    arr[0xa9] = { LDA, Immediate, 2 };
    arr[0xaa] = { TAX, Implied, 2 };
    arr[0xac] = { LDY, Absolute, 4 };
    arr[0xad] = { LDA, Absolute, 4 };
    arr[0xae] = { LDX, Absolute, 4 };

    arr[0xb0] = { BCS, Relative, 2 };
    arr[0xb1] = { LDA, IndirectIndexedY, 5 };
    arr[0xb4] = { LDY, ZeroPageX, 4 };
    arr[0xb5] = { LDA, ZeroPageX, 4 };
    arr[0xb6] = { LDX, ZeroPageY, 4 };
    arr[0xb8] = { CLV, Implied, 2 };
    arr[0xb9] = { LDA, AbsoluteY, 4 };
    arr[0xba] = { TSX, Implied, 2 };
    arr[0xbc] = { LDY, AbsoluteX, 4 };
    arr[0xbd] = { LDA, AbsoluteX, 4 };
    arr[0xbe] = { LDX, AbsoluteY, 4 };


    arr[0xc0] = { CPY, Immediate, 2 };
    arr[0xc1] = { CMP, IndexedIndirectX, 6 };
    arr[0xc4] = { CPY, ZeroPage, 3 };
    arr[0xc5] = { CMP, ZeroPage, 3 };
    arr[0xc6] = { DEC, ZeroPage, 5 };
    arr[0xc8] = { INY, Implied, 2 };
    arr[0xc9] = { CMP, Immediate, 2 };
    arr[0xca] = { DEX, Implied, 2 };
    arr[0xcc] = { CPY, Absolute, 4 };
    arr[0xcd] = { CMP, Absolute, 4 };
    arr[0xce] = { DEC, Absolute, 6 };

    arr[0xd0] = { BNE, Relative, 2 };
    arr[0xd1] = { CMP, IndirectIndexedY, 5 };
    arr[0xd5] = { CMP, ZeroPageX, 4 };
    arr[0xd6] = { DEC, ZeroPageX, 6 };
    arr[0xd8] = { CLD, Implied, 2 };
    arr[0xd9] = { CMP, AbsoluteY, 4 };
    arr[0xde] = { CMP, AbsoluteX, 4 };
    arr[0xdf] = { DEC, AbsoluteX, 7 };

    arr[0xe0] = { CPX, Immediate, 2 };
    arr[0xe1] = { SBC, IndexedIndirectX, 6 };
    arr[0xe4] = { CPX, ZeroPage, 3 };
    arr[0xe5] = { SBC, ZeroPage, 3 };
    arr[0xe6] = { INC, ZeroPage, 5 };
    arr[0xe8] = { INX, Implied, 2 };
    arr[0xe9] = { SBC, Immediate, 2 };
    arr[0xea] = { NOP, Implied, 2 };
    arr[0xec] = { CPX, Absolute, 4 };
    arr[0xed] = { SBC, Absolute, 4 };
    arr[0xee] = { INC, Absolute, 6 };

    arr[0xf0] = { BEQ, Relative, 2 };
    arr[0xf1] = { SBC, IndirectIndexedY, 5 };
    arr[0xf5] = { SBC, ZeroPageX, 4 };
    arr[0xf6] = { INC, ZeroPageX, 6 };
    arr[0xf8] = { SED, Implied, 2 };
    arr[0xf9] = { SBC, AbsoluteY, 4 };
    arr[0xfd] = { SBC, AbsoluteX, 4 };
    arr[0xfe] = { INC, AbsoluteX, 7 };

    return arr;
}();

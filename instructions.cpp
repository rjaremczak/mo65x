#include "instructions.h"

namespace cpu {

const InstructionsArray Instructions = []{
    InstructionsArray arr;
    arr.fill({Invalid});
    arr[0x00] = { BRK, Imp, 7 };
    arr[0x01] = { ORA, IndX, 6 };
    arr[0x05] = { ORA, Zp, 3 };
    arr[0x06] = { ASL, Zp, 5 };
    arr[0x08] = { PHP, Imp, 3 };
    arr[0x09] = { ORA, Imm, 2 };
    arr[0x0a] = { ASL, Acc, 2 };
    arr[0x0d] = { ORA, Abs, 4 };
    arr[0x0e] = { ASL, Abs, 6 };

    arr[0x10] = { BPL, Rel, 2 };
    arr[0x11] = { ORA, IndY, 5 };
    arr[0x15] = { ORA, ZpX, 4 };
    arr[0x16] = { ASL, ZpX, 6 };
    arr[0x18] = { CLC, Imp, 2 };
    arr[0x19] = { ORA, AbsY, 4 };
    arr[0x1d] = { ORA, AbsX, 4 };
    arr[0x1e] = { ASL, AbsX, 7 };

    arr[0x20] = { JSR, Abs, 6 };
    arr[0x21] = { AND, IndX, 6 };
    arr[0x24] = { BIT, Zp, 3 };
    arr[0x25] = { AND, Zp, 3 };
    arr[0x26] = { ROL, Zp, 5 };
    arr[0x28] = { PLP, Imp, 4 };
    arr[0x29] = { AND, Imm, 2 };
    arr[0x2a] = { ROL, Acc, 2 };
    arr[0x2c] = { BIT, Abs, 4 };
    arr[0x2d] = { AND, Abs, 4 };
    arr[0x2e] = { ROL, Abs, 6 };

    arr[0x30] = { BMI, Rel, 2 };
    arr[0x31] = { AND, IndY, 5 };
    arr[0x35] = { AND, ZpX, 4 };
    arr[0x36] = { ROL, ZpX, 6 };
    arr[0x38] = { SEC, Imp, 2 };
    arr[0x39] = { AND, AbsY, 4 };
    arr[0x3d] = { AND, AbsX, 4 };
    arr[0x3e] = { ROL, AbsX, 7 };

    arr[0x40] = { RTI, Imp, 6 };
    arr[0x41] = { EOR, IndX, 6 };
    arr[0x45] = { EOR, Zp, 3 };
    arr[0x46] = { LSR, Zp, 5 };
    arr[0x48] = { PHA, Imp, 3 };
    arr[0x49] = { EOR, Imm, 2 };
    arr[0x4a] = { LSR, Acc, 2 };
    arr[0x4c] = { JMP, Abs, 3 };
    arr[0x4d] = { EOR, Abs, 4 };
    arr[0x4e] = { LSR, Abs, 6 };

    arr[0x50] = { BVC, Rel, 2 };
    arr[0x51] = { EOR, IndY, 5 };
    arr[0x55] = { EOR, ZpX, 4 };
    arr[0x56] = { LSR, ZpX, 6 };
    arr[0x58] = { CLI, Imp, 2 };
    arr[0x59] = { EOR, AbsY, 4 };
    arr[0x5d] = { EOR, AbsX, 4 };
    arr[0x5e] = { LSR, AbsX, 7 };

    arr[0x60] = { RTS, Imp, 6 };
    arr[0x61] = { ADC, IndX, 6 };
    arr[0x65] = { ADC, Zp, 3 };
    arr[0x66] = { ROR, Zp, 5 };
    arr[0x68] = { PLA, Imp, 4 };
    arr[0x69] = { ADC, Imm, 2 };
    arr[0x6a] = { ROR, Acc, 2 };
    arr[0x6c] = { JMP, Ind, 6 };
    arr[0x6d] = { ADC, Abs, 4 };
    arr[0x6e] = { ROR, Abs, 6 };

    arr[0x70] = { BVS, Rel, 2 };
    arr[0x71] = { ADC, IndY, 5 };
    arr[0x75] = { ADC, ZpX, 4 };
    arr[0x76] = { ROR, ZpX, 6 };
    arr[0x78] = { SEI, Imp, 2 };
    arr[0x79] = { ADC, AbsY, 4 };
    arr[0x7d] = { ADC, AbsX, 4 };
    arr[0x7e] = { ROR, AbsX, 7 };


    arr[0x81] = { STA, IndX, 6 };
    arr[0x84] = { STY, Zp, 3 };
    arr[0x85] = { STA, Zp, 3 };
    arr[0x86] = { STX, Zp, 3 };
    arr[0x88] = { DEY, Imp, 2 };
    arr[0x8a] = { TXA, Imp, 2 };
    arr[0x8c] = { STY, Abs, 4 };
    arr[0x8d] = { STA, Abs, 4 };
    arr[0x8e] = { STX, Abs, 4 };

    arr[0x90] = { BCC, Rel, 2 };
    arr[0x91] = { STA, IndY, 6 };
    arr[0x94] = { STY, ZpX, 4 };
    arr[0x95] = { STA, ZpX, 4 };
    arr[0x96] = { STX, ZpY, 4 };
    arr[0x98] = { TYA, Imp, 2 };
    arr[0x99] = { STA, AbsY, 5 };
    arr[0x9a] = { TXS, Imp, 2 };
    arr[0x9d] = { STA, AbsX, 5 };

    arr[0xa0] = { LDY, Imm, 2 };
    arr[0xa1] = { LDA, IndX, 6 };
    arr[0xa2] = { LDX, Imm, 2 };
    arr[0xa4] = { LDY, Zp, 3 };
    arr[0xa5] = { LDA, Zp, 3 };
    arr[0xa6] = { LDX, Zp, 3 };
    arr[0xa8] = { TAY, Imp, 2 };
    arr[0xa9] = { LDA, Imm, 2 };
    arr[0xaa] = { TAX, Imp, 2 };
    arr[0xac] = { LDY, Abs, 4 };
    arr[0xad] = { LDA, Abs, 4 };
    arr[0xae] = { LDX, Abs, 4 };

    arr[0xb0] = { BCS, Rel, 2 };
    arr[0xb1] = { LDA, IndY, 5 };
    arr[0xb4] = { LDY, ZpX, 4 };
    arr[0xb5] = { LDA, ZpX, 4 };
    arr[0xb6] = { LDX, ZpY, 4 };
    arr[0xb8] = { CLV, Imp, 2 };
    arr[0xb9] = { LDA, AbsY, 4 };
    arr[0xba] = { TSX, Imp, 2 };
    arr[0xbc] = { LDY, AbsX, 4 };
    arr[0xbd] = { LDA, AbsX, 4 };
    arr[0xbe] = { LDX, AbsY, 4 };


    arr[0xc0] = { CPY, Imm, 2 };
    arr[0xc1] = { CMP, IndX, 6 };
    arr[0xc4] = { CPY, Zp, 3 };
    arr[0xc5] = { CMP, Zp, 3 };
    arr[0xc6] = { DEC, Zp, 5 };
    arr[0xc8] = { INY, Imp, 2 };
    arr[0xc9] = { CMP, Imm, 2 };
    arr[0xca] = { DEX, Imp, 2 };
    arr[0xcc] = { CPY, Abs, 4 };
    arr[0xcd] = { CMP, Abs, 4 };
    arr[0xce] = { DEC, Abs, 6 };

    arr[0xd0] = { BNE, Rel, 2 };
    arr[0xd1] = { CMP, IndY, 5 };
    arr[0xd5] = { CMP, ZpX, 4 };
    arr[0xd6] = { DEC, ZpX, 6 };
    arr[0xd8] = { CLD, Imp, 2 };
    arr[0xd9] = { CMP, AbsY, 4 };
    arr[0xde] = { CMP, AbsX, 4 };
    arr[0xdf] = { DEC, AbsX, 7 };

    arr[0xe0] = { CPX, Imm, 2 };
    arr[0xe1] = { SBC, IndX, 6 };
    arr[0xe4] = { CPX, Zp, 3 };
    arr[0xe5] = { SBC, Zp, 3 };
    arr[0xe6] = { INC, Zp, 5 };
    arr[0xe8] = { INX, Imp, 2 };
    arr[0xe9] = { SBC, Imm, 2 };
    arr[0xea] = { NOP, Imp, 2 };
    arr[0xec] = { CPX, Abs, 4 };
    arr[0xed] = { SBC, Abs, 4 };
    arr[0xee] = { INC, Abs, 6 };

    arr[0xf0] = { BEQ, Rel, 2 };
    arr[0xf1] = { SBC, IndY, 5 };
    arr[0xf5] = { SBC, ZpX, 4 };
    arr[0xf6] = { INC, ZpX, 6 };
    arr[0xf8] = { SED, Imp, 2 };
    arr[0xf9] = { SBC, AbsY, 4 };
    arr[0xfd] = { SBC, AbsX, 4 };
    arr[0xfe] = { INC, AbsX, 7 };

    return arr;
}();

}

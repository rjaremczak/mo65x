#pragma once

#include <cstdint>

namespace cpu {

enum AddressingMode
{
    Imp, Acc, Imm, Rel, Abs, AbX, AbY, Zpg, ZpX, ZpY, Ind, InX, InY
};

enum Mnemonic
{
    ADC, SBC, AND, ORA, ASL, LSR, EOR, ROL, ROR,
    BIT, CMP, CPX, CPY,
    INC, INX, INY,
    DEC, DEX, DEY,
    BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS,
    CLC, CLD, CLI, CLV,
    SEC, SED, SEI,
    JMP, JSR, BRK, RTI, RTS,
    LDA, LDX, LDY,
    STA, STX, STY,
    TAX, TAY, TSX, TXA, TYA, TXS,
    PHA, PHP, PLA, PLP,
    NOP,
    Invalid
};

struct Instruction
{
    const uint8_t opCode;
    const Mnemonic mnemonic;
    const AddressingMode addressingMode;
    const uint8_t bytes;
    const uint8_t cycles;
};

static constexpr Instruction NotSupported { 0, Invalid, Imp, 0, 0 };

extern const Instruction Instructions[];

}

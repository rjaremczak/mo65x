#pragma once

#include <cstdint>
#include <array>

namespace cpu {

struct AddressingMode
{
    uint8_t size;
};

const AddressingMode
    Imp{0}, Acc{0},
    Rel{1}, Imm{1}, Zp{1}, ZpX{1}, ZpY{1}, IndX{1}, IndY{1},
    Ind{2}, Abs{2}, AbsX{2}, AbsY{2};

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
    Mnemonic mnemonic = Invalid;
    AddressingMode addressingMode = Imp;
    uint8_t cycles = 0;
};

using InstructionsArray = std::array<Instruction, 256>;

extern const InstructionsArray Instructions;

}

#pragma once

#include <cstdint>
#include <array>


enum AddressingMode {
    Imp, Acc,
    Rel, Imm, Zp, ZpX, ZpY, IndX, IndY,
    Ind, Abs, AbsX, AbsY
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
    Mnemonic mnemonic = Invalid;
    AddressingMode addressingMode = Imp;
    uint8_t bytes = 1;
    uint8_t cycles = 0;

    Instruction() = default;
    Instruction(Mnemonic mnemonic, AddressingMode addressingMode, uint8_t cycles);
};

using InstructionsArray = std::array<Instruction, 256>;

extern const InstructionsArray Instructions;

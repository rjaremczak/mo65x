#pragma once

#include <cstdint>

enum Addressing {
    Implied, Accumulator,
    Relative, Immediate, ZeroPage, ZeroPageX, ZeroPageY, IndexedIndirectX, IndirectIndexedY,
    Indirect, Absolute, AbsoluteX, AbsoluteY
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
    Addressing addressing = Implied;
    uint8_t size = 1;
    uint8_t cycles = 0;

    Instruction() = default;
    Instruction(Mnemonic mnemonic, Addressing addressing, uint8_t cycles);
};

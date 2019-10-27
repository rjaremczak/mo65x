#pragma once

enum InstructionType
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
    INV
};
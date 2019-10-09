#pragma once

#include <QString>

struct Mnemonic {
    const char* code;
    const char* description;

};

constexpr Mnemonic
    ADC {"ADC", "Add with Carry"},
    AND {"AND", "And"},
    ASL {"ASL", "Arithmetic Shift Left"},
    BIT {"BIT", "Bit Test"},
    EOR {"EOR", "Exclusive-Or"},
    ORA {"ORA", "Or Accumulator"},

    CMP {"CMP", "Compare with Accumulator"},
    CPX {"CPX", "Compare with X"},
    CPY {"CPY", "Compare with Y"},

    INC {"INC", "Increment Memory by One"},
    INX {"INX", "Increment X by One"},
    INY {"INY", "Increment Y by One"},

    DEC {"DEC", "Decrement Memory by One"},
    DEX {"DEX", "Decrement X by One"},
    DEY {"DEY", "Decrement Y by One"},

    BCC {"BCC", "Branch on Carry Clear"},
    BCS {"BCS", "Branch on Carry Set"},
    BEQ {"BEQ", "Branch on Equal"},
    BMI {"BMI", "Branch on Minus"},
    BNE {"BNE", "Branch on Not Equal"},
    BPL {"BPL", "Branch on Plus"},
    BVC {"BVC", "Branch on Overflow Clear"},
    BVS {"BVS", "Branch on Overflow Set"},

    CLC {"CLC", "Clear Carry Flag"},
    CLD {"CLD", "Clear Decimal Mode"},
    CLI {"CLI", "Clear Interrupt Disable Bit"},
    CLV {"CLV", "Clear Overflow Flag"},


    BRK {"BRK", "Break"},
    NOP {"NOP", "No Operation"}
    ;

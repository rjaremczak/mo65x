#pragma once

#include <QString>
#include "memory.h"
#include "instruction.h"

class Disassembler
{
    const Memory& memory_;
    uint16_t address_;
    uint16_t opcode_;
    Instruction instruction_;

    QString formatOperand8() const;
    QString formatOperand16() const;

public:
    Disassembler(const Memory&, uint16_t addr = 0);

    auto address() const { return address_; }
    void setAddress(uint16_t addr);
    void step();

    QString dumpBytes(uint16_t n = 1) const;
    QString dumpWords(uint16_t n = 1) const;
    QString disassemble() const;
};

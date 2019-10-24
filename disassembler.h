#pragma once

#include <QString>
#include "memory.h"
#include "instruction.h"

class Disassembler
{
    const Memory& m_memory;
    uint16_t m_addr;
    uint16_t m_opcode;
    Instruction m_operation;

    QString formatOperand8() const;
    QString formatOperand16() const;

public:
    Disassembler(const Memory&, uint16_t addr = 0);

    void setAddr(uint16_t addr);
    void step();

    QString dumpBytes(uint16_t n = 1) const;
    QString dumpWords(uint16_t n = 1) const;
    QString disassemble() const;
};

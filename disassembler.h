#pragma once

#include <QString>
#include "memory.h"
#include "instructions.h"

class Disassembler
{
    const Memory& m_memory;
    uint16_t m_addr;
    Instruction m_instruction;

public:
    Disassembler(const Memory&, uint16_t addr = 0);

    auto addr() const { return m_addr; }
    void setAddr(uint16_t addr) { m_addr = addr; }
    void forward(int n = 1);
    void step();

    QString dumpBytes(int n = 1) const;
    QString dumpWords(int n = 1) const;
    QString disassemble() const;
};

#pragma once

#include <cstdint>
#include "memory.h"

class MemoryPointer
{
    Memory& m_memory;
    uint16_t m_address;

public:
    MemoryPointer(Memory&);

    uint16_t get() const { return m_address; }
    void set(uint16_t address) { m_address = address; }
    void inc(int i = 1) { m_address += i; }
};

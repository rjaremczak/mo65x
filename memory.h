#pragma once

#include <cstdint>

class Memory
{
    uint8_t m_ram[65536];

public:
    Memory();

    uint8_t read8(uint16_t addr) const
    {
        return m_ram[addr];
    }

    uint16_t read16(uint16_t addr) const
    {
        return static_cast<uint16_t>(m_ram[addr] | m_ram[static_cast<uint16_t>(addr + 1)] << 8);
    }

    void write8(uint16_t addr, uint8_t val)
    {
        m_ram[addr] = val;
    }

    void write16(uint16_t addr, uint16_t val)
    {
        m_ram[addr] = val & 0xff;
        m_ram[static_cast<uint16_t>(addr + 1)] = val >> 8;
    }
};

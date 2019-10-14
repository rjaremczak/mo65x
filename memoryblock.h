#pragma once

#include <cstdint>

template <uint16_t StartAddr, uint16_t EndAddr>
class MemoryBlock
{
private:
    uint8_t m_data[EndAddr - StartAddr];

    uint16_t index(uint16_t addr) const { return addr - StartAddr; }

public:
    uint8_t read8(uint16_t addr) const
    {
        return m_data[addr - StartAddr];
    }

    void write8(uint16_t addr, uint8_t byte)
    {
        m_data[addr - StartAddr] = byte;
    }
};

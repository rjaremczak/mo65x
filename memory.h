#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct Memory : public std::array<uint8_t, 65536> {
  uint16_t read16(uint16_t addr) const {
    return static_cast<uint16_t>((*this)[addr] | (*this)[static_cast<uint16_t>(addr + 1)] << 8);
  }

  void write16(uint16_t addr, uint16_t val) {
    (*this)[addr] = val & 0xff;
    (*this)[static_cast<uint16_t>(addr + 1)] = val >> 8;
  }
};

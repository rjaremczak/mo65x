#pragma once

#include "commondefs.h"
#include <iterator>

class Memory {
public:
  static constexpr size_t Size = 0x10000;

  auto size() const { return Size; }

  uint8_t& operator[](Address address) { return data[address]; }
  const uint8_t& operator[](Address address) const { return data[address]; }

  auto begin() { return std::begin(data); }
  auto end() { return std::end(data); }

  auto cbegin() const { return std::cbegin(data); }
  auto cend() const { return std::cend(data); }

  uint16_t word(Address addr) const { return static_cast<uint16_t>(data[addr] | data[addr + 1] << 8); }

  void setWord(Address addr, uint16_t val) {
    data[addr] = static_cast<uint8_t>(val);
    data[addr + 1] = val >> 8;
  }

private:
  uint8_t data[Size];
};

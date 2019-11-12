#pragma once

#include <iterator>

class Memory {
public:
  static constexpr size_t Size = 0x10000;

  auto size() const { return Size; }

  uint8_t& operator[](uint16_t address) { return data_[address]; }
  const uint8_t& operator[](uint16_t address) const { return data_[address]; }

  auto begin() { return std::begin(data_); }
  auto end() { return std::end(data_); }

  auto cbegin() const { return std::cbegin(data_); }
  auto cend() const { return std::cend(data_); }

  uint16_t read16(uint16_t addr) const { return static_cast<uint16_t>(data_[addr] | data_[addr + 1] << 8); }

  void write16(uint16_t addr, uint16_t val) {
    data_[addr] = static_cast<uint8_t>(val);
    data_[addr + 1] = val >> 8;
  }

  template <typename Container>
  void writeData(uint16_t addr, const Container container) {
    for (const auto& b : container) {
      data_[addr++] = b;
    }
  }

private:
  uint8_t data_[Size];
};

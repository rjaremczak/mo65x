#pragma once

#include <algorithm>
#include <cstdint>

struct AddressRange {
  uint16_t first;
  uint16_t last;

  AddressRange() : first(0), last(0) {}
  AddressRange(uint16_t addr) : first(addr), last(addr) {}
  AddressRange(uint16_t first, uint16_t last) : first(std::min(first, last)), last(std::max(first, last)) {}

  bool valid() const { return first >= last; }
  size_t size() const { return last - first + 1; }
  bool overlapsWith(AddressRange range) const {
    return first == std::clamp(first, range.first, range.last) || last == std::clamp(last, range.first, range.last);
  }
};

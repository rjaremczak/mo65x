#pragma once

#include "defs.h"
#include <algorithm>
#include <cstdint>

struct AddressRange {
  Address first;
  Address last;

  AddressRange() : first(0xffff), last(0) {}
  AddressRange(Address addr) : first(addr), last(addr) {}
  AddressRange(Address first, Address last) : first(first), last(last) {}

  bool valid() const { return first <= last; }
  size_t size() const { return last - first + 1; }
  bool contains(Address addr) const { return addr == std::clamp(addr, first, last); }
  bool overlapsWith(AddressRange range) const { return range.contains(first) || range.contains(last); }

  void expand(Address addr) {
    if (valid()) {
      first = std::min(first, addr);
      last = std::max(last, addr);
    } else {
      first = last = addr;
    }
  }
};

#include "symboltable.h"

bool SymbolTable::put(const std::string_view& name, uint16_t value) {
  if (get(name)) return false;
  operator[](std::string(name)) = value;
  return true;
}

std::optional<int> SymbolTable::get(const std::string_view& name) const {
  for (const auto& entry : *this) {
    if (entry.first == name) return entry.second;
  }
  return std::nullopt;
}

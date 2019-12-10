#include "symboltable.h"

bool SymbolTable::put(const QString& name, uint16_t value) {
  if (find(name) != end()) return false;
  operator[](name) = value;
  return true;
}

std::optional<int> SymbolTable::get(const QString& name) const {
  if (const auto it = find(name); it != end()) return it->second;
  return std::nullopt;
}

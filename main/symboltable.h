#pragma once

#include <map>
#include <optional>
#include <string>

struct SymbolTable : std::map<std::string, uint16_t> {
  bool contains(const std::string_view& name) const;
  bool put(const std::string_view& name, uint16_t value);
  std::optional<int> get(const std::string_view& name) const;
};

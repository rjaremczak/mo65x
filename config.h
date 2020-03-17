#pragma once

#include <nlohmann/json.hpp>

struct Config {
  std::string asmFileName;

  void read(const nlohmann::json&);
  nlohmann::json toJson() const;
};

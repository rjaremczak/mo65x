#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <filesystem>

template <class DataObjectType>
class FileDataStorage {
  const std::filesystem::path m_path;

 public:
  FileDataStorage(const std::filesystem::path&& path) : m_path(std::move(path)) {}

  bool read(DataObjectType& dao) {
    std::ifstream is(m_path);
    if (!is.is_open()) return false;

    std::ostringstream oss;
    oss << is.rdbuf();
    dao.read(nlohmann::json::parse(oss.str()));
    return true;
  }

  DataObjectType read() {
    DataObjectType dataObject;
    read(dataObject);
    return dataObject;
  }

  void write(const DataObjectType& dao) {
    std::ofstream os(m_path);
    if (os.is_open()) os << dao.toJson().dump(4);
  }

  DataObjectType readOrInit() {
    DataObjectType dao;
    if (!read(dao)) write(dao);
    return dao;
  }
};

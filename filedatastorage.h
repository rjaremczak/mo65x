#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>

template <class DataObjectType> class FileDataStorage {
  const std::string m_filePath;

public:
  FileDataStorage(const std::string& filePath) : m_filePath(filePath) {}

  bool read(DataObjectType& dao) {
    std::ifstream is(m_filePath);
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
    std::ofstream os(m_filePath);
    if (os.is_open()) os << dao.toJson().dump(4);
  }

  DataObjectType readOrInit() {
    DataObjectType dao;
    if (!read(dao)) write(dao);
    return dao;
  }
};

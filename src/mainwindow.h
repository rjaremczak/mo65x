#pragma once

#include "config.h"
#include "emulator.h"
#include "filedatastorage.h"
#include <filesystem>

class MainWindow
{
public:
  MainWindow();

private:
  FileDataStorage<Config>* m_configStorage;
  Config m_config;
  Emulator m_emulator;

  void initConfigStorage();
  void loadMemoryFromFile(Address start, const std::filesystem::path& fpath);
  void saveMemoryToFile(AddressRange, const std::filesystem::path& fpath);
};

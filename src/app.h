#pragma once

#include "config.h"
#include "emulator.h"
#include "filedatastorage.h"
#include "renderwindow.h"

class App {
 public:
  App();
  int run();
  void errorMessage(const std::string&);

 private:
  std::unique_ptr<FileDataStorage<Config>> m_configStorage;
  std::unique_ptr<RenderWindow> m_renderWindow;
  Config m_config;
  Emulator m_emulator;

  void initConfigStorage();
  void initRenderWindow();
  void loadMemoryFromFile(Address start, const std::filesystem::path& fpath);
  void saveMemoryToFile(AddressRange, const std::filesystem::path& fpath);
};

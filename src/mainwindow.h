#pragma once

#include "config.h"
#include "emulator.h"
#include "filedatastorage.h"

class MainWindow
{
public:
  MainWindow();

private:
  FileDataStorage<Config>* m_configStorage;
  Config m_config;
  Emulator m_emulator;

  void initConfigStorage();
};

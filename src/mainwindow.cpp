#include "mainwindow.h"
#include "commonutils.h"

static const auto ProjectName = "mo65x";
static const auto ConfigFileName = "config.json";
static const auto ProjectVersion = "1.01";

MainWindow::MainWindow() {
  initConfigStorage();
}

void MainWindow::initConfigStorage() {
  const auto appDir = mindpart::home_directory() / (std::string(".") + ProjectName);
  if (!std::filesystem::exists(appDir)) std::filesystem::create_directory(appDir);
  m_configStorage = new FileDataStorage<Config>(appDir / ConfigFileName);
  m_config = m_configStorage->readOrInit();
}

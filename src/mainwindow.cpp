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

void MainWindow::loadMemoryFromFile(Address start, const std::filesystem::path& fpath) {
  std::ifstream is(fpath, std::ios::in | std::ios::binary);
  if(is.is_open()) {
    const auto size = std::filesystem::file_size(fpath);
    Data buf(static_cast<size_t>(size));
    if(is.read(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(size))) {
      m_emulator.writeMemory(start, buf);
      return;
    }
  }
  //showMessage(rsize > 0 ? tr("loaded %1 B\nfrom file %2").arg(rsize).arg(fname) : "load error", rsize > 0);
}

void MainWindow::saveMemoryToFile(AddressRange range, const std::filesystem::path& fpath) {
  std::ofstream os(fpath, std::ios::out | std::ios::binary);
  if(os.is_open()) {
    Data buf(range.size());
    m_emulator.readMemory(range, buf);
    if(os.write(reinterpret_cast<char*>(buf.data()), static_cast<std::streamsize>(buf.size()))) return;
  }
  //showMessage(rsize > 0 ? tr("saved %1 B\nto file %2").arg(rsize).arg(fname) : "save error", rsize > 0);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assemblerwidget.h"
#include "centralwidget.h"
#include "config.h"
#include "cpuwidget.h"
#include "disassemblerwidget.h"
#include "emulator.h"
#include "filedatastorage.h"
#include "memorywidget.h"
#include "videowidget.h"
#include <QMainWindow>
#include <QThread>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

  Ui::MainWindow* ui;

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

public slots:
  void changeAsmFileName(const QString&);
  void showMessage(const QString& message, bool success = true);
  void prepareToQuit();

private:
  CentralWidget* m_viewWidget;
  AssemblerWidget* m_assemblerWidget;
  MemoryWidget* m_memoryWidget;
  DisassemblerWidget* m_disassemblerWidget;
  CpuWidget* m_cpuWidget;
  VideoWidget* m_videoWidget;
  FileDataStorage<Config>* m_configStorage;
  Config m_config;
  QTimer* m_pollTimer;
  Emulator m_emulator;

  void initConfigStorage();
  void propagateState(EmulatorState);
  void emulatorStateChanged();
  void emulatorMemoryContentChanged(AddressRange);

private slots:
  void polling();
  void clearStatistics();
  void stopExecution();
  void triggerReset();
  void triggerNmi();
  void triggerIrq();

  // former EmulatorQt slots

  void execute(bool continuous, Frequency clock);
  void setRegisterPC(Address);
  void setRegisterSP(Address);
  void setRegisterA(uint8_t);
  void setRegisterX(uint8_t);
  void setRegisterY(uint8_t);
  void memoryContentChanged(AddressRange);
  void loadMemoryFromFile(Address start, const QString& fname);
  void saveMemoryToFile(AddressRange, const QString& fname);
};

#endif // MAINWINDOW_H

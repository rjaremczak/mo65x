#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "assemblerwidget.h"
#include "centralwidget.h"
#include "config.h"
#include "cpuwidget.h"
#include "disassemblerwidget.h"
#include "emulator.h"
#include "emulatorqt.h"
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
  CentralWidget* viewWidget;
  AssemblerWidget* assemblerWidget;
  MemoryWidget* memoryWidget;
  DisassemblerWidget* disassemblerWidget;
  CpuWidget* cpuWidget;
  VideoWidget* videoWidget;
  EmulatorQt* emulator;
  FileDataStorage<Config>* configStorage;
  Config config;
  QTimer* pollTimer;
  QThread emulatorThread;

  Emulator m_emulator;

  void initConfigStorage();
  void startEmulator();
  void propagateState(EmulatorState);
  void emulatorStateChanged();

private slots:
  void polling();
  void clearStatistics();
  void stopExecution();
  void triggerReset();
  void triggerNmi();
  void triggerIrq();
};

#endif // MAINWINDOW_H

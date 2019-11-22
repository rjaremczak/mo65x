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
  Emulator* emulator;
  FileDataStorage<Config>* configStorage;
  Config config;
  QTimer* pollTimer;
  QThread emulatorThread;

  void initConfigStorage();
  void startEmulator();
  void propagateState(EmulatorState);

private slots:
  void polling();
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "centralwidget.h"
#include "cpuwidget.h"
#include "memorywidget.h"
#include "system.h"

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

private:
  MemoryWidget* memoryWidget;
  CentralWidget* centralWidget;
  CpuWidget* cpuWidget;
  QTimer* pollTimer;
  System* system;
};

#endif // MAINWINDOW_H

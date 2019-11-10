#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "assemblerwidget.h"
#include "cpuwidget.h"
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
  AssemblerWidget* assemblerWidget;
  CpuWidget* cpuWidget;
  QTimer* pollTimer;
  System* system;
};

#endif // MAINWINDOW_H

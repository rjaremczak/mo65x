#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "centralwidget.h"
#include "executionwidget.h"
#include "memorywidget.h"
#include "monitorwidget.h"
#include "system.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MemoryWidget* memoryWidget;
    MonitorWidget* monitorWidget;
    CentralWidget* centralWidget;
    ExecutionWidget* executionWidget;
    QTimer* pollTimer;
    System* system;
};

#endif // MAINWINDOW_H

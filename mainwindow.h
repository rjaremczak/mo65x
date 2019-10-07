#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "cpuwidget.h"
#include "monitorwidget.h"
#include "emulator.h"

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
    CpuWidget* m_cpuWidget = nullptr;
    MonitorWidget* m_monitorWidget = nullptr;
    QTimer* m_pollTimer = nullptr;
    Emulator* m_emulator = nullptr;
};

#endif // MAINWINDOW_H

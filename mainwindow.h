#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "cpuwidget.h"
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
    QTimer* m_pollTimer = nullptr;
    Emulator* m_cpu = nullptr;
};

#endif // MAINWINDOW_H

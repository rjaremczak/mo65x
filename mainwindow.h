#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "monitorwidget.h"
#include "memorywidget.h"
#include "centralwidget.h"
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
    MemoryWidget* m_memoryWidget;
    MonitorWidget* m_monitorWidget;
    CentralWidget* m_centralWidget;
    QTimer* m_pollTimer;
    System* m_system;
};

#endif // MAINWINDOW_H

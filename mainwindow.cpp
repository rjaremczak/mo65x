#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_emulator = new Board(this);

    m_cpuWidget = new CpuWidget();
    this->addDockWidget(Qt::RightDockWidgetArea, m_cpuWidget);
    connect(m_emulator, &Board::cpuRegistersChanged, m_cpuWidget, &CpuWidget::updateState);

    m_monitorWidget = new MonitorWidget();
    this->addDockWidget(Qt::RightDockWidgetArea, m_monitorWidget);

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, m_emulator, &Board::checkCpuRegisters);
    m_pollTimer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}


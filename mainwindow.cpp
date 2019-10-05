#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_cpu = new Emulator(this);

    m_cpuWidget = new CpuWidget();
    this->addDockWidget(Qt::RightDockWidgetArea, m_cpuWidget);
    connect(m_cpu, &Emulator::cpuStateChanged, m_cpuWidget, &CpuWidget::updateState);

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, m_cpu, &Emulator::checkCpuState);
    m_pollTimer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}


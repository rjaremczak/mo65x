#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_system = new System(this);

    m_memoryWidget = new MemoryWidget(this, m_system);
    this->addDockWidget(Qt::RightDockWidgetArea, m_memoryWidget);

    m_monitorWidget = new MonitorWidget(this, m_system->memory());
    this->addDockWidget(Qt::RightDockWidgetArea, m_monitorWidget);

    m_pollTimer = new QTimer(this);
    connect(m_pollTimer, &QTimer::timeout, m_system, &System::checkCpuState);
    m_pollTimer->start(1000);

    connect(m_system, &System::cpuStateChanged, m_monitorWidget, &MonitorWidget::updateCpuState);
    connect(m_system, &System::memoryContentChanged, m_monitorWidget, &MonitorWidget::updateMemoryView);

    connect(m_monitorWidget, &MonitorWidget::addressChanged, m_system, &System::changePC);

    m_monitorWidget->changeAddress(m_system->cpu().pc());
}

MainWindow::~MainWindow()
{
    delete ui;
}


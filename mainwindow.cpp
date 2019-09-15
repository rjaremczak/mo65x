#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_cpuWidget = new CpuWidget();
    this->addDockWidget(Qt::RightDockWidgetArea, m_cpuWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}


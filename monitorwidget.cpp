#include "monitorwidget.h"
#include "ui_monitorwidget.h"

MonitorWidget::MonitorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MonitorWidget)
{
    ui->setupUi(this);
}

MonitorWidget::~MonitorWidget()
{
    delete ui;
}

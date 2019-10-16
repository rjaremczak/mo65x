#include "memorywidget.h"
#include "ui_memorywidget.h"

MemoryWidget::MemoryWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::MemoryWidget)
{
    ui->setupUi(this);
}

MemoryWidget::~MemoryWidget()
{
    delete ui;
}

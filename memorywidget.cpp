#include "memorywidget.h"
#include "ui_memorywidget.h"

MemoryWidget::MemoryWidget(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::MemoryWidget)
{
  ui->setupUi(this);
}

MemoryWidget::~MemoryWidget()
{
  delete ui;
}

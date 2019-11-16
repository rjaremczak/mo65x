#include "memorywidget.h"
#include "ui_memorywidget.h"
#include "uitools.h"
#include <QMessageBox>

enum Operation { Load, Save };

MemoryWidget::MemoryWidget(QWidget* parent, const Memory& memory) : QWidget(parent), ui(new Ui::MemoryWidget), memory_(memory) {
  ui->setupUi(this);
  setMonospaceFont(ui->dumpView);
  ui->operationSelector->insertItem(Load, "Load");
  ui->operationSelector->insertItem(Save, "Save");
  connect(ui->executeOperation, &QToolButton::clicked, this, &MemoryWidget::executeOperation);
  connect(ui->operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MemoryWidget::prepareOperation);

  ui->operationSelector->setCurrentIndex(Save);
}

MemoryWidget::~MemoryWidget() {
  delete ui;
}

void MemoryWidget::updateMemoryView(quint16 first, quint16 last) {
}

void MemoryWidget::loadFile(const QString& fname) {
}

void MemoryWidget::saveFile(const QString& fname) {
}

void MemoryWidget::prepareOperation(int sel) {
  switch (static_cast<Operation>(sel)) {
  case Load:
    ui->executeOperation->setText("From File...");
    ui->fromLabel->setText("At");
    ui->toLabel->setVisible(false);
    ui->endAddress->setVisible(false);
    break;
  case Save:
    ui->executeOperation->setText("To File...");
    ui->fromLabel->setText("From");
    ui->toLabel->setVisible(true);
    ui->endAddress->setVisible(true);
    break;
  }
}

void MemoryWidget::executeOperation() {
  QMessageBox::information(this, "", "exec");
}

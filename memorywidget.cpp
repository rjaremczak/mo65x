#include "memorywidget.h"
#include "ui_memorywidget.h"
#include "uitools.h"
#include <QMessageBox>
#include <QResizeEvent>

enum Operation { Load, Save };

MemoryWidget::MemoryWidget(QWidget* parent, const Memory& memory) : QWidget(parent), ui(new Ui::MemoryWidget), memory_(memory) {
  ui->setupUi(this);
  ui->operationSelector->insertItem(Load, "Load");
  ui->operationSelector->insertItem(Save, "Save");
  connect(ui->executeOperation, &QToolButton::clicked, this, &MemoryWidget::executeOperation);
  connect(ui->operationSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MemoryWidget::prepareOperation);
  setMonospaceFont(ui->textView);
  ui->operationSelector->setCurrentIndex(Save);
}

MemoryWidget::~MemoryWidget() {
  delete ui;
}

void MemoryWidget::updateMemoryView(AddressRange range) {
  if (viewRange_.overlapsWith(range)) updateMemoryView();
}

void MemoryWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) { updateMemoryView(); }
}

void MemoryWidget::loadFile(const QString& fname) {
}

void MemoryWidget::saveFile(const QString& fname) {
}

void MemoryWidget::updateMemoryView() {
  QString html("<div style='white-space:pre; display:inline-block; color:darkseagreen'>");
  int rows = rowsInView();
  int cols = colsInView();
  uint16_t addr = viewRange_.first;
  for (int row = 0; row < rows; row++) {
    html.append(formatHexWord(addr).toUpper()).append("   ");
    for (int x = 0; x < (cols - 7) / 3; x++) { html.append(formatHexByte(memory_[addr++]).toUpper()).append(" "); }
    html.append("<br>");
  }
  html.append("</div>");
  ui->textView->setHtml(html);
  viewRange_.last = addr - 1;
}

int MemoryWidget::rowsInView() const {
  return 6 + ui->textView->height() / ui->textView->fontMetrics().height();
}

int MemoryWidget::colsInView() const {
  return ui->textView->width() / ui->textView->fontMetrics().horizontalAdvance('0');
}

void MemoryWidget::prepareOperation(int sel) {
  switch (static_cast<Operation>(sel)) {
  case Load:
    ui->executeOperation->setText("From File...");
    ui->endLabel->setVisible(false);
    ui->endAddress->setVisible(false);
    break;
  case Save:
    ui->executeOperation->setText("To File...");
    ui->endLabel->setVisible(true);
    ui->endAddress->setVisible(true);
    break;
  }
}

void MemoryWidget::executeOperation() {
  QMessageBox::information(this, "", "exec");
}

void MemoryWidget::changeStartAddress(uint16_t) {
}

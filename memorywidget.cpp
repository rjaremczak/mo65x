#include "memorywidget.h"
#include "formatters.h"
#include "ui_memorywidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QResizeEvent>

MemoryWidget::MemoryWidget(QWidget* parent, const Memory& memory) : QWidget(parent), ui(new Ui::MemoryWidget), memory_(memory) {
  ui->setupUi(this);
  connect(ui->loadFromFile, &QAbstractButton::clicked, this, &MemoryWidget::loadFromFile);
  connect(ui->saveToFile, &QAbstractButton::clicked, this, &MemoryWidget::saveToFile);
  connect(ui->startAddress, QOverload<int>::of(&QSpinBox::valueChanged), this, &MemoryWidget::changeStartAddress);
  connect(ui->endAddress, QOverload<int>::of(&QSpinBox::valueChanged), this, &MemoryWidget::changeEndAddress);
  setMonospaceFont(ui->textView);
  setMonospaceFont(ui->startAddress);
  setMonospaceFont(ui->endAddress);
}

MemoryWidget::~MemoryWidget() {
  delete ui;
}

void MemoryWidget::updateMemory(AddressRange range) {
  if (addressRange_.overlapsWith(range)) updateView();
}

void MemoryWidget::updatePolledData(EmulatorState, AddressRange range) {
  updateMemory(range);
}

void MemoryWidget::resizeEvent(QResizeEvent* event) {
  if (event->size() != event->oldSize()) { updateView(); }
}

void MemoryWidget::updateView() {
  QString html("<div style='white-space:pre; display:inline-block; color:gray'>");
  int rows = rowsInView() - 1;
  int cols = colsInView();
  uint16_t addr = addressRange_.first;
  for (int row = 0; row < rows; row++) {
    html.append("<div>");
    html.append(formatHexWord(addr).toUpper()).append(" <span style='color:lightgreen'>");
    for (int x = 0; x < (cols - 7) / 3; x++) { html.append(formatHexByte(memory_[addr++]).toUpper()).append(" "); }
    html.append("</span></div>");
  }
  html.append("</div>");
  ui->textView->setHtml(html);
  addressRange_.last = addr - 1;
}

int MemoryWidget::rowsInView() const {
  return ui->textView->height() / ui->textView->fontMetrics().height();
}

int MemoryWidget::colsInView() const {
  return ui->textView->width() / ui->textView->fontMetrics().horizontalAdvance('0');
}

void MemoryWidget::loadFromFile() {
  if (auto fname = QFileDialog::getOpenFileName(this, tr("Open Binary File")); !fname.isEmpty()) {
    emit loadFromFileRequested(static_cast<uint16_t>(ui->startAddress->value()), fname);
  }
}

void MemoryWidget::saveToFile() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save Binary File")); !fname.isEmpty()) {
    emit saveToFileRequested({static_cast<uint16_t>(ui->startAddress->value()), static_cast<uint16_t>(ui->endAddress->value())},
                             fname);
  }
}

void MemoryWidget::changeStartAddress(uint16_t addr) {
  if (addressRange_.first != addr) {
    addressRange_.first = addr;
    updateView();
  }
  ui->endAddress->setMinimum(addressRange_.first + 1);
}

void MemoryWidget::changeEndAddress(uint16_t addr) {
  ui->endAddress->setValue(std::max(addressRange_.first, addr));
}

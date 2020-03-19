#include "memorywidget.h"
#include "commonformatters.h"
#include "ui_memorywidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QResizeEvent>

MemoryWidget::MemoryWidget(QWidget* parent, const Memory& memory) : QWidget(parent), ui(new Ui::MemoryWidget), memory(memory) {
  ui->setupUi(this);
  connect(ui->loadFromFile, &QAbstractButton::clicked, this, &MemoryWidget::loadFromFile);
  connect(ui->saveToFile, &QAbstractButton::clicked, this, &MemoryWidget::saveToFile);
  connect(ui->startAddress, QOverload<int>::of(&QSpinBox::valueChanged), this, &MemoryWidget::changeStartAddress);
  connect(ui->endAddress, QOverload<int>::of(&QSpinBox::valueChanged), this, &MemoryWidget::changeEndAddress);
  setMonospaceFont(ui->textView);
  setMonospaceFont(ui->startAddress);
  setMonospaceFont(ui->endAddress);

  ui->startAddress->setValue(0);
  changeStartAddress(0);
}

MemoryWidget::~MemoryWidget() {
  delete ui;
}

void MemoryWidget::updateOnMemoryChange(AddressRange range) {
  if (addressRange.overlapsWith(range)) updateView();
}

void MemoryWidget::resizeEvent(QResizeEvent* event) {
  if (event->size() != event->oldSize()) { updateView(); }
}

void MemoryWidget::updateView() {
  QString html("<div style='white-space:pre; display:inline-block; color:gray'>");
  int rows = rowsInView() - 1;
  int cols = colsInView();
  auto addr = addressRange.first;
  for (int row = 0; row < rows; row++) {
    html.append("<div>");
    html.append(formatHexWord(addr).toUpper()).append(" <span style='color:lightgreen'>");
    for (int x = 0; x < (cols - 7) / 3; x++) {
      html.append(formatHexByte(memory[addr++]).toUpper()).append(" ");
    }
    html.append("</span></div>");
  }
  html.append("</div>");
  ui->textView->setHtml(html);
  addressRange.last = addr - 1;
  changeEndAddress(addressRange.last);
}

int MemoryWidget::rowsInView() const {
  return ui->textView->height() / ui->textView->fontMetrics().height();
}

int MemoryWidget::colsInView() const {
  return ui->textView->width() / ui->textView->fontMetrics().horizontalAdvance('0');
}

void MemoryWidget::loadFromFile() {
  if (auto fname = QFileDialog::getOpenFileName(this, tr("Open Binary File")); !fname.isEmpty()) {
    emit loadFromFileRequested(static_cast<Address>(ui->startAddress->value()), fname);
  }
}

void MemoryWidget::saveToFile() {
  if (auto fname = QFileDialog::getSaveFileName(this, tr("Save Binary File")); !fname.isEmpty()) {
    emit saveToFileRequested({static_cast<Address>(ui->startAddress->value()), static_cast<Address>(ui->endAddress->value())},
                             fname);
  }
}

void MemoryWidget::changeStartAddress(Address addr) {
  if (addressRange.first != addr) {
    addressRange.first = addr;
    updateView();
  }
  ui->endAddress->setMinimum(addressRange.first);
}

void MemoryWidget::changeEndAddress(Address addr) {
  ui->endAddress->setValue(std::max(addressRange.first, addr));
}

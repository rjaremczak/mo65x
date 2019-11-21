#include "disassemblerwidget.h"
#include "ui_disassemblerwidget.h"
#include "uitools.h"
#include <QVBoxLayout>

DisassemblerWidget::DisassemblerWidget(QWidget* parent, const Memory& memory) : QWidget(parent), ui(new Ui::DisassemblerWidget) {
  ui->setupUi(this);

  view = new DisassemblerView(this, memory, DisassemblerView::HighlightMode::Selected);
  layout()->addWidget(view);
  connect(ui->startAddress, QOverload<int>::of(&QSpinBox::valueChanged), view, &DisassemblerView::changeStart);
  connect(ui->goToStart, &QAbstractButton::clicked, [&] { emit goToStartClicked(view->first()); });
  connect(ui->goToSelection, &QAbstractButton::clicked, [&] { ui->startAddress->setValue(view->selected()); });
  view->changeStart(static_cast<Address>(ui->startAddress->value()));
}

DisassemblerWidget::~DisassemblerWidget() {
  delete ui;
}

void DisassemblerWidget::updatePolledData(EmulatorState state, AddressRange range) {
  updateState(state);
  updateMemory(range);
}

void DisassemblerWidget::updateState(EmulatorState state) {
  view->changeSelected(state.regs.pc);
}

void DisassemblerWidget::updateMemory(AddressRange range) {
  view->updateMemoryView(range);
}

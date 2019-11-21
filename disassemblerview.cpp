#include "disassemblerview.h"
#include "formatters.h"
#include "ui_disassemblerview.h"
#include "uitools.h"
#include <QResizeEvent>

DisassemblerView::DisassemblerView(QWidget* parent, const Memory& memory, HighlightMode highlight)
    : QWidget(parent), ui(new Ui::DisassemblerView), disassembler(memory), highlightMode(highlight) {
  ui->setupUi(this);
  setMonospaceFont(ui->view);
}

DisassemblerView::~DisassemblerView() {
  delete ui;
}

Address DisassemblerView::last() const {
  return addressRange.last;
}

Address DisassemblerView::selected() const {
  return selectedAddress;
}

uint16_t DisassemblerView::first() const {
  return addressRange.first;
}

void DisassemblerView::updateMemoryView(AddressRange range) {
  if (range.overlapsWith(range)) updateView();
}

void DisassemblerView::changeStart(Address addr) {
  if (addressRange.first != addr) {
    addressRange.first = addr;
    updateView();
  }
}

void DisassemblerView::changeSelected(Address addr) {
  if (selectedAddress != addr) {
    selectedAddress = addr;
    if (highlightMode != HighlightMode::None && addressRange.contains(selectedAddress)) updateView();
  }
}

void DisassemblerView::updateView() {
  disassembler.setOrigin(addressRange.first);
  QString html("<div style='white-space:pre; display:inline-block'>");
  int rows = rowsInView();
  while (rows--) {
    auto hl = shouldHighlightCurrentAddress();
    html.append(hl ? "<div style='color:black; background-color: lightgreen'>" : "<div style='color:darkseagreen'>");
    html.append(hl ? "<span style='color:black'>" : "<span style='color:gray'>");
    html.append(formatHexWord(disassembler.currentAddress()).toUpper());
    html.append("</span> ");
    html.append(disassembler.disassemble());
    html.append("</div>");
    disassembler.nextInstruction();
  }
  html.append("</div>");
  ui->view->setHtml(html);
  addressRange.last = disassembler.currentAddress();
}

void DisassemblerView::nextInstruction() {
  disassembler.setOrigin(addressRange.first);
  disassembler.nextInstruction();
  changeStart(disassembler.currentAddress());
}

void DisassemblerView::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) { updateView(); }
}

int DisassemblerView::rowsInView() const {
  return 6 + ui->view->height() / ui->view->fontMetrics().height();
}

bool DisassemblerView::shouldHighlightCurrentAddress() const {
  if (highlightMode == HighlightMode::None) return false;

  const auto addr = disassembler.currentAddress();
  return (highlightMode == HighlightMode::First && addr == addressRange.first) ||
         (highlightMode == HighlightMode::Selected && addr == selectedAddress);
}

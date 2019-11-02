#include "disassemblerwidget.h"
#include "disassembler.h"
#include "ui_disassemblerwidget.h"
#include <QFontDatabase>
#include <QResizeEvent>

DisassemblerWidget::DisassemblerWidget(QWidget* parent, System* system)
    : QDockWidget(parent), ui(new Ui::DisassemblerWidget), system(system), disassembler(system->memoryView()) {
  ui->setupUi(this);
  initView();
  connect(ui->skipInstruction, &QToolButton::clicked, this, &DisassemblerWidget::skipInstruction);
}

DisassemblerWidget::~DisassemblerWidget() {
  delete ui;
}

void DisassemblerWidget::changeAddress(uint16_t pc) {
  if (firstAddress != pc) {
    firstAddress = pc;
    updateMemoryView();
    emit addressChanged(pc);
  }
}

void DisassemblerWidget::updateMemoryContent(uint16_t first, uint16_t last) {
  if (firstAddress == std::clamp(firstAddress, first, last) || lastAddress == std::clamp(lastAddress, first, last)) {
    updateMemoryView();
  }
}

void DisassemblerWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) {
    updateMemoryView();
  }
}

int DisassemblerWidget::rowsInView() const {
  return 2 + ui->dumpView->height() / ui->dumpView->fontMetrics().height();
}

void DisassemblerWidget::initView() {
  ui->viewModeGroup->setId(ui->viewModeAsm, Asm);
  ui->viewModeGroup->setId(ui->viewModeHex, Hex);
  ui->viewModeGroup->setId(ui->viewModeTxt, Txt);
  ui->viewModeAsm->setChecked(true);
}

void DisassemblerWidget::disassemblerView() {
  disassembler.setAddress(firstAddress);
  QString html("<div style='white-space:pre; display:inline-block'>");
  int rows = rowsInView();
  if (rows--) {
    html.append("<div style='color:black; background-color: lightgreen'>");
    html.append(disassembler.disassemble()).append("</div>");
    disassembler.nextInstruction();

    html.append("<div style='color:darkseagreen'>");
    while (rows--) {
      html.append(disassembler.disassemble() + "<br>");
      disassembler.nextInstruction();
    }

    html.append("</div>");
  }
  html.append("</div>");
  ui->dumpView->setHtml(html);
  lastAddress = disassembler.currentAddress();
}

void DisassemblerWidget::updateMemoryView() {
  disassemblerView();
}

void DisassemblerWidget::skipInstruction() {
  disassembler.setAddress(firstAddress);
  disassembler.nextInstruction();
  changeAddress(disassembler.currentAddress());
}

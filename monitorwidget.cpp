#include "monitorwidget.h"
#include "disassembler.h"
#include "ui_monitorwidget.h"
#include <QFontDatabase>
#include <QResizeEvent>

MonitorWidget::MonitorWidget(QWidget* parent, System* system)
    : QDockWidget(parent), ui(new Ui::MonitorWidget), system(system), disassembler(system->memoryView()) {
  ui->setupUi(this);
  initView();
  connect(ui->skipInstruction, &QToolButton::clicked, this, &MonitorWidget::skipInstruction);
}

MonitorWidget::~MonitorWidget() {
  delete ui;
}

void MonitorWidget::changeAddress(uint16_t pc) {
  if (firstAddress != pc) {
    firstAddress = pc;
    updateMemoryView();
    emit addressChanged(pc);
  }
}

void MonitorWidget::updateMemoryContent(uint16_t first, uint16_t last) {
  if (firstAddress == std::clamp(firstAddress, first, last) || lastAddress == std::clamp(lastAddress, first, last)) {
    updateMemoryView();
  }
}

void MonitorWidget::resizeEvent(QResizeEvent* event) {
  if (event->size().height() != event->oldSize().height()) {
    updateMemoryView();
  }
}

int MonitorWidget::rowsInView() const {
  return 2 + ui->dumpView->height() / ui->dumpView->fontMetrics().height();
}

void MonitorWidget::initView() {
  ui->viewModeGroup->setId(ui->viewModeAsm, Asm);
  ui->viewModeGroup->setId(ui->viewModeHex, Hex);
  ui->viewModeGroup->setId(ui->viewModeTxt, Txt);
  ui->viewModeAsm->setChecked(true);

  auto font = QFontDatabase::systemFont(QFontDatabase::FixedFont);

#ifdef __APPLE__
  font.setPointSize(13);
#endif

  ui->dumpView->setFont(font);
}

void MonitorWidget::disassemblerView() {
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

void MonitorWidget::updateMemoryView() {
  disassemblerView();
}

void MonitorWidget::skipInstruction() {
  disassembler.setAddress(firstAddress);
  disassembler.nextInstruction();
  changeAddress(disassembler.currentAddress());
}

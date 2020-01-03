#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(QWidget* parent, QWidget* assembler, QWidget* memory, QWidget* disassembler)
    : QWidget(parent), ui(new Ui::CentralWidget), assemblerWidget(assembler), memoryWidget(memory),
      disassemblerWidget(disassembler) {
  ui->setupUi(this);
  ui->stackedWidget->addWidget(assemblerWidget);
  ui->stackedWidget->addWidget(memoryWidget);
  ui->stackedWidget->addWidget(disassemblerWidget);
  connect(ui->assemblerViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(assemblerWidget); });
  connect(ui->memoryViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(memoryWidget); });
  connect(ui->disassemblerViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(disassemblerWidget); });
  ui->assemblerViewButton->click();
}

bool CentralWidget::isVisible(const QWidget* w) const {
  return ui->stackedWidget->currentWidget() == w;
}

CentralWidget::~CentralWidget() {
  delete ui;
}

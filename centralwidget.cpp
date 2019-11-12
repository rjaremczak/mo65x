#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(QWidget* parent, QWidget* assembler, QWidget* memory)
    : QWidget(parent), ui(new Ui::CentralWidget), assemblerWidget_(assembler), memoryWidget_(memory) {
  ui->setupUi(this);
  ui->stackedWidget->addWidget(assemblerWidget_);
  ui->stackedWidget->addWidget(memoryWidget_);

  connect(ui->assemblerViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(assemblerWidget_); });
  connect(ui->memoryViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(memoryWidget_); });

  ui->assemblerViewButton->click();
}

CentralWidget::~CentralWidget() {
  delete ui;
}

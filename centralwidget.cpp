#include "centralwidget.h"
#include "ui_centralwidget.h"

CentralWidget::CentralWidget(QWidget* parent, QWidget* assembler, QWidget* memory)
    : QWidget(parent), ui(new Ui::CentralWidget), assemblerWidget(assembler), memoryWidget(memory) {
  ui->setupUi(this);
  ui->stackedWidget->addWidget(assemblerWidget);
  ui->stackedWidget->addWidget(memoryWidget);

  connect(ui->assemblerViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(assemblerWidget); });
  connect(ui->memoryViewButton, &QToolButton::clicked, [&] { ui->stackedWidget->setCurrentWidget(memoryWidget); });

  ui->assemblerViewButton->click();
}

CentralWidget::~CentralWidget() {
  delete ui;
}

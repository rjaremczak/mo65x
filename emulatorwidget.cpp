#include "emulatorwidget.h"
#include "ui_emulatorwidget.h"

EmulatorWidget::EmulatorWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EmulatorWidget)
{
    ui->setupUi(this);
}

EmulatorWidget::~EmulatorWidget()
{
    delete ui;
}

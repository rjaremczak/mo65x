#include "videowidget.h"
#include "ui_videowidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget* parent, const Memory& memory) : QDockWidget(parent), ui(new Ui::VideoWidget), memory(memory) {
  ui->setupUi(this);
}

VideoWidget::~VideoWidget() {
  delete ui;
}

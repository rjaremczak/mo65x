#include "videowidget.h"
#include "ui_videowidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget* parent, const Memory& memory) : QDockWidget(parent), ui(new Ui::VideoWidget), memory(memory) {
  ui->setupUi(this);
  connect(ui->address, QOverload<int>::of(&QSpinBox::valueChanged),
          [&](Address addr) { ui->screen->setFrameBuffer(&memory[addr], ResolutionX, ResolutionY); });
}

VideoWidget::~VideoWidget() {
  delete ui;
}

void VideoWidget::setFrameBufferAddress(Address addr) {
  ui->address->setValue(addr);
}

void VideoWidget::refresh() {
  ui->screen->update();
}

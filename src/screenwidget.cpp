#include "screenwidget.h"
#include <QPainter>
#include <QRandomGenerator>

ScreenWidget::ScreenWidget(QWidget* parent) : QWidget(parent), colorTable(256) {
  setColorTableToC64Palette();
}

void ScreenWidget::setFrameBuffer(const uint8_t* buf, int resx, int resy) {
  if (frameBuffer != buf) {
    frameBuffer = buf;
    resolutionX = resx;
    resolutionY = resy;
    update();
  }
}

void ScreenWidget::paintEvent(QPaintEvent*) {
  if (!frameBuffer) return;

  QPainter painter(this);
  painter.setPen(Qt::gray);
  painter.setBrush(QBrush(Qt::black));

  QImage image(frameBuffer, resolutionX, resolutionY, QImage::Format_Indexed8);
  image.setColorTable(colorTable);

  painter.drawImage(rect(), image);
}

void ScreenWidget::setColorTableToC64Palette() {
  colorTable[0] = 0xFF000000;
  colorTable[1] = 0xFFFFFFFF;
  colorTable[2] = 0xFF880000;
  colorTable[3] = 0xFFAAFFEE;
  colorTable[4] = 0xFFCC44CC;
  colorTable[5] = 0xFF00CC55;
  colorTable[6] = 0xFF0000AA;
  colorTable[7] = 0xFFEEEE77;
  colorTable[8] = 0xFFDD8855;
  colorTable[9] = 0xFF664400;
  colorTable[10] = 0xFFFF7777;
  colorTable[11] = 0xFF333333;
  colorTable[12] = 0xFF777777;
  colorTable[13] = 0xFFAAFF66;
  colorTable[14] = 0xFF0088FF;
  colorTable[15] = 0xFFBBBBBB;
  for (int i = 1; i < 16; i++) std::copy_n(colorTable.begin(), 16, std::next(colorTable.begin(), i * 16));
}

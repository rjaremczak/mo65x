#include "screenwidget.h"
#include <QPainter>
#include <QRandomGenerator>

ScreenWidget::ScreenWidget(QWidget* parent) : QWidget(parent), colorTable(256) {
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
  for (auto& v : frameBuffer) v = static_cast<uint8_t>(QRandomGenerator::global()->bounded(255));
}

void ScreenWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setPen(Qt::gray);
  painter.setBrush(QBrush(Qt::black));
  // painter.drawRect(0, 0, 127, 127);

  QImage image(frameBuffer, FrameResolution.width(), FrameResolution.height(), QImage::Format_Indexed8);
  image.setColorTable(colorTable);

  painter.drawImage(rect(), image);
}

#pragma once

#include <QImage>
#include <QWidget>

class ScreenWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ScreenWidget(QWidget* parent = nullptr);

signals:

public slots:
  void setFrameBuffer(const uint8_t*, int resx, int resy);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  QVector<QRgb> colorTable;
  int resolutionX;
  int resolutionY;
  const uint8_t* frameBuffer = nullptr;

  void setColorTableToC64Palette();
};

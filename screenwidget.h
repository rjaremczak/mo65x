#pragma once

#include <QImage>
#include <QWidget>

class ScreenWidget : public QWidget
{
  Q_OBJECT
public:
  static constexpr QSize FrameResolution{32, 32};

  explicit ScreenWidget(QWidget* parent = nullptr);

signals:

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  QVector<QRgb> colorTable;
  uint8_t frameBuffer[FrameResolution.width() * FrameResolution.height()];
};

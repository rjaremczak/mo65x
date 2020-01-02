#pragma once

#include "memory.h"
#include <QDockWidget>

namespace Ui {
class VideoWidget;
}

class VideoWidget : public QDockWidget
{
  Q_OBJECT

public:
  static constexpr auto ResolutionX = 32;
  static constexpr auto ResolutionY = 32;

  explicit VideoWidget(QWidget* parent, const Memory& memory);
  ~VideoWidget();

public slots:
  void setFrameBufferAddress(Address addr);
  void refresh();

private:
  Ui::VideoWidget *ui;
  const Memory& memory;
  Address address;

  void fillWithNoise();
};

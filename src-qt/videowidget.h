#pragma once

#include "addressrange.h"
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
  void updateOnMemoryChange(AddressRange range);
  void updateView();

private:
  Ui::VideoWidget *ui;
  const Memory& memory;
  AddressRange addressRange;

  void fillWithNoise();
};

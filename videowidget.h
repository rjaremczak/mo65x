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
  explicit VideoWidget(QWidget* parent, const Memory&);
  ~VideoWidget();

private:
  Ui::VideoWidget *ui;
  const Memory& memory;
  Address address;

  void fillWithNoise();
};

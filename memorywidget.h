#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MemoryWidget(QWidget *parent = nullptr);
  ~MemoryWidget();

private:
  Ui::MemoryWidget *ui;
};

#endif // MEMORYWIDGET_H

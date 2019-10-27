#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include "system.h"
#include <QDockWidget>

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QDockWidget {
  Q_OBJECT

public:
  explicit MemoryWidget(QWidget* parent, System* system);
  ~MemoryWidget();

public slots:

private slots:
  void loadMemoryFromFile();
  void fillMemory();

private:
  Ui::MemoryWidget* ui;
  System* system_;
};

#endif // MEMORYWIDGET_H

#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include "memory.h"
#include <QWidget>

namespace Ui {
class MemoryWidget;
}

class MemoryWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MemoryWidget(QWidget* parent, const Memory&);
  ~MemoryWidget();

signals:
  void memoryContentChanged(quint16 first, quint16 last);

public slots:
  void updateMemoryView(quint16 first, quint16 last);

private:
  Ui::MemoryWidget* ui;
  const Memory& memory_;

  void loadFile(const QString& fname);
  void saveFile(const QString& fname);

private slots:
  void prepareOperation(int);
  void executeOperation();
};

#endif // MEMORYWIDGET_H

#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include "addressrange.h"
#include "commons.h"
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
  ~MemoryWidget() override;

signals:
  void memoryContentChanged(AddressRange);
  void loadFromFileRequested(uint16_t start, const QString& fname);
  void saveToFileRequested(AddressRange, const QString& fname);

public slots:
  void updateMemoryView(AddressRange);

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::MemoryWidget* ui;
  const Memory& memory_;
  AddressRange addressRange_;

  void updateView();
  int rowsInView() const;
  int colsInView() const;

private slots:
  void loadFromFile();
  void saveToFile();
  void changeStartAddress(uint16_t);
  void changeEndAddress(uint16_t);
};

#endif // MEMORYWIDGET_H

#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include "addressrange.h"
#include "commondefs.h"
#include "emulatorstate.h"
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
  void loadFromFileRequested(Address start, const QString& fname);
  void saveToFileRequested(AddressRange, const QString& fname);

public slots:
  void updateOnChange(AddressRange);

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::MemoryWidget* ui;
  const Memory& memory;
  AddressRange addressRange = AddressRange::Invalid;

  void updateView();
  int rowsInView() const;
  int colsInView() const;

private slots:
  void loadFromFile();
  void saveToFile();
  void changeStartAddress(Address);
};

#endif // MEMORYWIDGET_H

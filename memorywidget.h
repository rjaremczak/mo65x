#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

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
  ~MemoryWidget();

signals:
  void memoryContentChanged(uint16_t first, uint16_t last);
  void loadFromFileRequested(uint16_t start, QString fname, std::function<void(qint64)> callback);

public slots:
  void updateMemoryView(AddressRange);

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::MemoryWidget* ui;
  const Memory& memory_;
  AddressRange viewRange_;

  void updateMemoryView();
  int rowsInView() const;
  int colsInView() const;

private slots:
  void prepareOperation(int);
  void executeOperation();
  void changeStartAddress(uint16_t);
};

#endif // MEMORYWIDGET_H

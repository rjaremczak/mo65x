#ifndef DISASSEMBLERWIDGET_H
#define DISASSEMBLERWIDGET_H

#include "commondefs.h"
#include "disassemblerview.h"
#include "emulatorstate.h"
#include "memory.h"
#include <QWidget>

namespace Ui {
class DisassemblerWidget;
}

class DisassemblerWidget : public QWidget
{
  Q_OBJECT

public:
  DisassemblerView* view;

  explicit DisassemblerWidget(QWidget* parent, const Memory&);
  ~DisassemblerWidget();

signals:
  void goToStartClicked(Address);

public slots:
  void updatePolledData(EmulatorState, AddressRange);

private:
  Ui::DisassemblerWidget* ui;
};

#endif // DISASSEMBLERWIDGET_H

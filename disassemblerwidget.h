#ifndef DISASSEMBLERWIDGET_H
#define DISASSEMBLERWIDGET_H

#include "defs.h"
#include "disassemblerview.h"
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

private:
  Ui::DisassemblerWidget* ui;
};

#endif // DISASSEMBLERWIDGET_H

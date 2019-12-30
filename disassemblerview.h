#ifndef DISASSEMBLERVIEW_H
#define DISASSEMBLERVIEW_H

#include "addressrange.h"
#include "commondefs.h"
#include "disassembler.h"
#include "memory.h"
#include <QWidget>

namespace Ui {
class DisassemblerView;
}

class DisassemblerView : public QWidget
{
  Q_OBJECT

public:
  enum class HighlightMode { None, First, Selected };

  explicit DisassemblerView(QWidget* parent, const Memory& memory, HighlightMode highligt = HighlightMode::First);
  ~DisassemblerView() override;
  Address first() const;
  Address last() const;
  Address selected() const;

public slots:
  void updateMemoryView(AddressRange);
  void changeStart(Address);
  void changeSelected(Address);
  void updateView();
  void nextInstruction();

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::DisassemblerView* ui;

  Disassembler disassembler;
  AddressRange addressRange = AddressRange::Invalid;
  Address selectedAddress;
  HighlightMode highlightMode;

  int rowsInView() const;
  bool shouldHighlightCurrentAddress() const;
};

#endif // DISASSEMBLERVIEW_H

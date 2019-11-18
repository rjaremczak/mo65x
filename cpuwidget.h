#pragma once

#include "commons.h"
#include "cpuinfo.h"
#include "disassembler.h"
#include <QDockWidget>

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget {
  Q_OBJECT

public:
  explicit CpuWidget(QWidget* parent, const Memory&);
  ~CpuWidget() override;

signals:
  void executeOneInstructionRequested();
  void continuousExecutionRequested();
  void stopExecutionRequested();
  void programCounterChanged(uint16_t);

public slots:
  void updateMemoryView(AddressRange);
  void updateState(CpuInfo);
  void updateDisassemblerView();
  void changeProgramCounter(uint16_t);

protected:
  void resizeEvent(QResizeEvent*) override;

private:
  Ui::CpuWidget* ui;

  const Memory& memory_;
  Disassembler disassembler_;
  AddressRange disassemblerRange_;

  int rowsInView() const;

private slots:
  void skipInstruction();
};

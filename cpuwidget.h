#pragma once

#include "addressrange.h"
#include "commons.h"
#include "cpuinfo.h"
#include "disassembler.h"
#include <QDockWidget>

namespace Ui {
class CpuWidget;
}

class CpuWidget : public QDockWidget {
  Q_OBJECT
  Q_ENUM(ExecutionState)

public:
  explicit CpuWidget(QWidget* parent, const Memory&);
  ~CpuWidget() override;

signals:
  void executeOneInstructionRequested();
  void startExecutionRequested();
  void stopExecutionRequested();
  void clearCycleCounterRequested();
  void resetRequested();
  void nmiRequested();
  void irqRequested();
  void programCounterChanged(uint16_t);
  void stackPointerChanged(uint16_t);
  void registerAChanged(uint8_t);
  void registerXChanged(uint8_t);
  void registerYChanged(uint8_t);

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

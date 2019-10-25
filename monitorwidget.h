#ifndef MONITORWIDGET_H
#define MONITORWIDGET_H

#include "disassembler.h"
#include "system.h"
#include <QDockWidget>
#include <QStringListModel>

namespace Ui {
class MonitorWidget;
}

class MonitorWidget : public QDockWidget {
  Q_OBJECT

public:
  enum ViewMode { Asm, Hex, Txt };

  explicit MonitorWidget(QWidget *parent, const Memory &);
  ~MonitorWidget() override;

signals:
  void addressChanged(uint16_t);

public slots:
  void changeAddress(uint16_t);
  void updateMemoryContent(uint16_t start, uint16_t last);
  void updateCpuState(CpuRegisters, CpuFlags);

protected:
  void resizeEvent(QResizeEvent *) override;

private:
  Ui::MonitorWidget *ui;

  Disassembler m_disassembler;
  uint16_t m_firstAddress;
  uint16_t m_lastAddress;

  int rowsInView() const;
  void initView();
  void disassemblerView();
  void updateMemoryView();
};

#endif // MONITORWIDGET_H

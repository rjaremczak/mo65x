#pragma once

#include "assembler.h"
#include <QWidget>

namespace Ui {
class AssemblerWidget;
}

class AssemblerWidget : public QWidget {
  Q_OBJECT

public:
  explicit AssemblerWidget(QWidget* parent = nullptr);
  ~AssemblerWidget();

signals:
  void fileLoaded(const QString&);
  void fileSaved(const QString&);
  void machineCodeGenerated(uint16_t origin, const Assembler::Code& code);

public slots:
  void loadFile(const QString& fname);
  void saveFile(const QString& fname);

private:
  Ui::AssemblerWidget* ui;
  QString fileName_;
  Assembler assembler_;

  bool assemble(Assembler::Pass);

private slots:
  void loadEditorFile();
  void saveEditorFile();
  void saveEditorFileAs();
  void assembleSourceCode();
};

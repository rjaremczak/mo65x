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

public slots:
  void loadFile(const QString& fname);

private:
  Ui::AssemblerWidget* ui;
  QString fileName;

private slots:
  void loadFileDialog();
  void saveFileDialog();
  void assemble();
};

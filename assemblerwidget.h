#pragma once

#include <QWidget>

namespace Ui {
class AssemblerWidget;
}

class AssemblerWidget : public QWidget {
  Q_OBJECT

public:
  explicit AssemblerWidget(QWidget* parent = nullptr);
  ~AssemblerWidget();

private:
  Ui::AssemblerWidget* ui;
  QString fileName;

private slots:
  void loadFile();
  void saveFile();
};

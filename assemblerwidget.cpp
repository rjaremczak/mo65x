#include "assemblerwidget.h"
#include "ui_assemblerwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

AssemblerWidget::AssemblerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::AssemblerWidget) {
  ui->setupUi(this);
  connect(ui->loadFile, &QToolButton::clicked, this, &AssemblerWidget::loadFileDialog);
  connect(ui->saveFile, &QToolButton::clicked, this, &AssemblerWidget::saveFileDialog);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) { ui->assembly->setPlainText(QTextStream(&file).readAll()); }
}

void AssemblerWidget::loadFileDialog() {
  QFileDialog::getOpenFileContent("", [&](const QString fname, const QByteArray& fcontent) {
    QString title = tr("Load File");
    if (fname.isEmpty()) {
      QMessageBox::warning(this, title, tr("loading error"));
    } else {
      ui->assembly->setPlainText(fcontent);
      emit fileLoaded(fname);
    }
  });
}

void AssemblerWidget::saveFileDialog() {
  QString title = tr("Save File");
  if (const auto fname = QFileDialog::getSaveFileName(this, "", title); !fname.isNull()) {
    QFile file(fname);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream(&file) << ui->assembly->toPlainText();
      emit fileSaved(fname);
    }
  }
}

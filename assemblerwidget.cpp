#include "assemblerwidget.h"
#include "assembler.h"
#include "ui_assemblerwidget.h"
#include "uitools.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

AssemblerWidget::AssemblerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::AssemblerWidget) {
  ui->setupUi(this);
  connect(ui->loadFile, &QToolButton::clicked, this, &AssemblerWidget::loadFileDialog);
  connect(ui->saveFile, &QToolButton::clicked, this, &AssemblerWidget::saveFileDialog);
  connect(ui->assembleSourceCode, &QToolButton::clicked, this, &AssemblerWidget::assembleSourceCode);
  setMonospaceFont(ui->sourceCode);
}

AssemblerWidget::~AssemblerWidget() {
  delete ui;
}

void AssemblerWidget::loadFile(const QString& fname) {
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    ui->sourceCode->setPlainText(QTextStream(&file).readAll());
    emit fileLoaded(fname);
  }
}

void AssemblerWidget::loadFileDialog() {
  QFileDialog::getOpenFileContent("", [&](const QString fileName, const QByteArray& fileContent) {
    QString title = tr("Load File");
    if (fileName.isEmpty()) {
      QMessageBox::warning(this, title, tr("loading error"));
    } else {
      ui->sourceCode->setPlainText(fileContent);
      emit fileLoaded(fileName);
    }
  });
}

void AssemblerWidget::saveFileDialog() {
  QString title = tr("Save File");
  if (const auto fname = QFileDialog::getSaveFileName(this, "", title); !fname.isNull()) {
    QFile file(fname);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream(&file) << ui->sourceCode->toPlainText();
      emit fileSaved(fname);
    }
  }
}

void AssemblerWidget::assembleSourceCode() {
  assembler.reset();
  QString src = ui->sourceCode->toPlainText();
  QTextStream is(&src, QIODevice::ReadOnly);
  int lineNum = 1;
  while (!is.atEnd()) {
    const auto line = is.readLine();
    if (line.isNull()) break;
    assembler.assemble(line);
  }
}

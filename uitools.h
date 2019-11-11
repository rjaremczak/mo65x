#pragma once

#include <QWidget>

void adjustMonospaceFont([[maybe_unused]] QWidget* widget) {
#ifdef Q_OS_MACOS
  widget->setFont(QFont("Monaco"));
#endif
}

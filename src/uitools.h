#pragma once

#include <QWidget>

inline void setMonospaceFont([[maybe_unused]] QWidget* widget) {
#ifdef Q_OS_MACOS
  widget->setFont(QFont("Andale Mono"));
#endif
}

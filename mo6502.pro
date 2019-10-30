QT       += core gui widgets

CONFIG += c++17
CONFIG += strict_c++
CONFIG += sdk_no_version_check

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    assembler.cpp \
    bytespinbox.cpp \
    centralwidget.cpp \
    cpu.cpp \
    disassembler.cpp \
    disassemblerwidget.cpp \
    executionwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    memory.cpp \
    memorywidget.cpp \
    system.cpp \
    wordspinbox.cpp

HEADERS += \
    assembler.h \
    decodetable.h \
    disassemblerwidget.h \
    executionstatus.h \
    addressingmode.h \
    bytespinbox.h \
    centralwidget.h \
    cpu.h \
    disassembler.h \
    executionwidget.h \
    instruction.h \
    instructiontable.h \
    instructiontype.h \
    mainwindow.h \
    memory.h \
    memorywidget.h \
    processorstatus.h \
    registers.h \
    stackpointer.h \
    system.h \
    utilities.h \
    wordspinbox.h

FORMS += \
    centralwidget.ui \
    disassemblerwidget.ui \
    executionwidget.ui \
    mainwindow.ui \
    memorywidget.ui

RESOURCES += resources.qrc

# Default rules for deployment.
# qnx: target.path = /tmp/$${TARGET}/bin
# else: unix:!android: target.path = /opt/$${TARGET}/bin
# !isEmpty(target.path): INSTALLS += target

test {
  message(test build)

  QT += testlib
  TARGET = $${TARGET}_tests

  SOURCES -= main.cpp
  SOURCES += \
    test/opcodestest.cpp \
    test/main.cpp

  HEADERS += test/opcodestest.h

} else {
  message(normal build)
}

message($${TARGET})

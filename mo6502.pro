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
    cpu.cpp \
    cpuregisters.cpp \
    cpuwidget.cpp \
    emulator.cpp \
    emulatorstate.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cpu.h \
    cpuregisters.h \
    cpuwidget.h \
    emulator.h \
    emulatorstate.h \
    mainwindow.h

FORMS += \
    cpuwidget.ui \
    mainwindow.ui

RESOURCES += resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

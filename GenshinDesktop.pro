QT       += core gui core5compat
QT += widgets
QT += network
QT += webenginewidgets
QT += webenginecore
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    genshin_setting.cpp \
    internetwindow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    genshin_setting.h \
    internetwindow.h \
    mainwindow.h

FORMS += \
    genshin_setting.ui \
    internetwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

#程序版本
VERSION = 1.0.2
#程序图标
RC_ICONS = ico.ico

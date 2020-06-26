#-------------------------------------------------
#
# Project created by QtCreator 2019-01-06T13:19:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyMH
TEMPLATE = app
RC_ICONS = app.ico

SOURCES += main.cpp\
        widget.cpp \
    visualboyadvance.cpp \
    uncode.cpp \
    cpu_id.cpp

HEADERS  += widget.h \
    pokemon.h \
    visualboyadvance.h

FORMS    += widget.ui

RESOURCES += \
    img.qrc

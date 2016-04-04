#-------------------------------------------------
#
# Project created by QtCreator 2016-04-01T18:40:39
#
#-------------------------------------------------

QT       += core gui \
            network \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Haptics
TEMPLATE = app

CONFIG(release, debug|release): LIBS += -lsfml-graphics -lsfml-main -lsfml-window

SOURCES += main.cpp\
        mainwindow.cpp \
    physics.cpp \
    visual.cpp \
    hapticinterface.cpp \
    qsfmlcanvas.cpp

HEADERS  += mainwindow.h \
    physics.h \
    visual.h \
    hapticinterface.h \
    qsfmlcanvas.h

FORMS    += mainwindow.ui

unix:!macx: LIBS += -lsfml-graphics -lsfml-window

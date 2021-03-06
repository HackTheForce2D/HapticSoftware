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

SOURCES += main.cpp\
        mainwindow.cpp \
    physics.cpp \
    visual.cpp \
    hapticinterface.cpp \
    qsfmlcanvas.cpp \
    body.cpp \
    effector.cpp \
    connectiondialog.cpp \
    createobject.cpp \
    pantograph.cpp

HEADERS  += mainwindow.h \
    physics.h \
    visual.h \
    hapticinterface.h \
    qsfmlcanvas.h \
    body.h \
    effector.h \
    connectiondialog.h \
    createobject.h \
    pantograph.h

FORMS    += mainwindow.ui \
    connectiondialog.ui \
    createobject.ui


unix:!macx: LIBS += -lsfml-graphics -lsfml-window -lsfml-system

unix:!macx: LIBS += -L$$PWD/lib/Box2D/ -lBox2D

INCLUDEPATH += $$PWD/lib/Box2D
DEPENDPATH += $$PWD/lib/Box2D

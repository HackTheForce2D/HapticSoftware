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

#CONFIG(release, debug|release): LIBS += -lBox2D -lsfml-graphics -lsfml-window -lsfml-system \

#unix:!macx: LIBS += -lsfml-graphics -lsfml-window -lsfml-system
#unix:!macx: LIBS +=



#INCLUDEPATH += /home/ygor/test_HTF_lib/SFML
#DEPENDPATH += /home/ygor/test_HTF_lib/SFML



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

#unix:!macx: LIBS += -lBox2D -lsfml-graphics -lsfml-window -lsfml-system



#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/SFML-2.3.2/lib/release/ -lsfml-graphics -lsfml-window -lsfml-system
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/SFML-2.3.2/lib/debug/ -lsfml-graphics -lsfml-window -lsfml-system
#else:unix: LIBS += -L$$PWD/SFML-2.3.2/lib/ -lsfml-graphics -lsfml-window -lsfml-system
#unix: LIBS += -L$$PWD/lib/SFML/ -lsfml-graphics -lsfml-window -lsfml-system

#INCLUDEPATH += $$PWD/include/SFML
#DEPENDPATH += $$PWD/include/SFML

#INCLUDEPATH += $$PWD/SFML-2.3.2/include
#DEPENDPATH += $$PWD/SFML-2.3.2/include


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Box2D/release/ -lBox2D
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Box2D/debug/ -lBox2D
#else:unix: LIBS += -L$$PWD/lib/Box2D/ -lBox2D

#INCLUDEPATH += $$PWD/include/Box2D
#DEPENDPATH += $$PWD/include/Box2D

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Box2D/release/libBox2D.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Box2D/debug/libBox2D.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Box2D/release/Box2D.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/Box2D/debug/Box2D.lib
#else:unix: PRE_TARGETDEPS += $$PWD/lib/Box2D/libBox2D.a




unix:!macx: LIBS += -lsfml-graphics -lsfml-window -lsfml-system

#INCLUDEPATH += $$PWD/../test_HTF_lib/SFML
#DEPENDPATH += $$PWD/../test_HTF_lib/SFML

unix:!macx: LIBS += -L$$PWD/lib/Box2D/ -lBox2D

INCLUDEPATH += $$PWD/lib/Box2D
DEPENDPATH += $$PWD/lib/Box2D

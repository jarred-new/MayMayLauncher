#-------------------------------------------------
#
# Project created by QtCreator 2026-06-11T11:17:35
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MayMayLauncher
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    musicplayer.cpp \
    videoplayer.cpp \
    pictureviewer.cpp

HEADERS  += mainwindow.h \
    musicplayer.h \
    videoplayer.h \
    pictureviewer.h \
    setup.h

FORMS    += mainwindow.ui \
    musicplayer.ui \
    videoplayer.ui \
    pictureviewer.ui

RESOURCES += \
    res.qrc

RC_FILE += \
    resource.rc

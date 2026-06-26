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
    pictureviewer.cpp \
    crashdialog.cpp \
    crashhandler.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    musicplayer.h \
    videoplayer.h \
    pictureviewer.h \
    setup.h \
    crashdialog.h \
    crashhandler.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    musicplayer.ui \
    videoplayer.ui \
    pictureviewer.ui \
    crashdialog.ui \
    settingsdialog.ui

RESOURCES += \
    res.qrc

RC_FILE += \
    resource.rc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libmagic/lib/ -lmagic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libmagic/lib/ -lmagicd

INCLUDEPATH += $$PWD/libmagic/include
DEPENDPATH += $$PWD/libmagic/include

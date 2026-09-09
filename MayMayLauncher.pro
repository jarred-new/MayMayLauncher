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

INCLUDEPATH += $$PWD/libmagic/include
DEPENDPATH += $$PWD/libmagic/include

win32 {
    # Direct path linking bypasses the -l short-name resolution issues
    CONFIG(release, debug|release): LIBS += $$PWD/libmagic/lib/magic.lib
    else:CONFIG(debug, debug|release): LIBS += $$PWD/libmagic/debug/lib/magic.lib
}


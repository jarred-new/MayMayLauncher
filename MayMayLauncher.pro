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
    midiplayer.cpp \
    musicplayer.cpp \
    videoplayer.cpp \
    pictureviewer.cpp \
    crashdialog.cpp \
    crashhandler.cpp \
    settingsdialog.cpp

HEADERS  += mainwindow.h \
    midiplayer.h \
    musicplayer.h \
    videoplayer.h \
    pictureviewer.h \
    setup.h \
    crashdialog.h \
    crashhandler.h \
    settingsdialog.h

FORMS    += mainwindow.ui \
    midiplayer.ui \
    musicplayer.ui \
    videoplayer.ui \
    pictureviewer.ui \
    crashdialog.ui \
    settingsdialog.ui

RESOURCES += \
    res.qrc

RC_FILE += \
    resource.rc

INCLUDEPATH += $$PWD/libmagic/include \
               $$PWD/fluidsynth/include
DEPENDPATH += $$PWD/libmagic/include \
              $$PWD/fluidsynth/include

win32 {
    # Direct path linking bypasses the -l short-name resolution issues
    CONFIG(release, debug|release): LIBS += $$PWD/libmagic/lib/magic.lib
    else:CONFIG(debug, debug|release): LIBS += $$PWD/libmagic/debug/lib/magic.lib
    LIBS        += "$$PWD/fluidsynth/lib/libfluidsynth-3.lib"

    CONFIG(release, debug|release) {
        QMAKE_POST_LINK += $$QMAKE_COPY "$$PWD/fluidsynth/bin/libfluidsynth-3.dll" "$$OUT_PWD/release" $$escape_expand(\n\t)
        QMAKE_POST_LINK += $$QMAKE_COPY "$$PWD/fluidsynth/bin/sndfile.dll" "$$OUT_PWD/release" $$escape_expand(\n\t)
        QMAKE_POST_LINK += $$QMAKE_COPY "$$PWD/fluidsynth/bin/SDL3.dll" "$$OUT_PWD/release"
    }
}


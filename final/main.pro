QT       += core gui
QT       += multimedia
QT       += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aitest.cpp \
    basic_variable.cpp \
    board.cpp \
    board_review.cpp \
    board_singleplayer.cpp \
    hiddensource.cpp \
    history.cpp \
    main.cpp \
    mainwindow.cpp \
    read_save_select.cpp \
    rule.cpp \
    shezhi.cpp

HEADERS += \
    aitest.h \
    basic_variable.h \
    board.h \
    board_review.h \
    board_singleplayer.h \
    hiddensource.h \
    history.h \
    mainwindow.h \
    read_save_select.h \
    rule.h \
    shezhi.h

FORMS += \
    board.ui \
    board_review.ui \
    board_singleplayer.ui \
    hiddensource.ui \
    history.ui \
    mainwindow.ui \
    read_save_select.ui \
    rule.ui \
    shezhi.ui

TRANSLATIONS += \
    test3_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    music.qrc \
    picture.qrc \
    wav.qrc

DISTFILES += \
    music/bgm1.mp3


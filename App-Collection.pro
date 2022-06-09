QT       += core gui multimedia multimediawidgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audiolevel.cpp \
    main.cpp \
    mainwindow.cpp \
    notepad.cpp \
    painter.cpp \
    recorder.cpp \
    scribblearea.cpp

HEADERS += \
    audiolevel.h \
    mainwindow.h \
    notepad.h \
    painter.h \
    recorder.h \
    scribblearea.h

FORMS += \
    mainwindow.ui \
    notepad.ui \
    painter.ui \
    recorder.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

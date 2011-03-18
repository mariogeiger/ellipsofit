# -------------------------------------------------
# Project created by QtCreator 2010-03-10T08:54:26
# -------------------------------------------------
QT += network
win32::TARGET = EllipsoFit
unix::TARGET = ellipsofit-fit
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    graphicsview.cpp \
    xyscene.cpp \
    paramedit.cpp \
    ellipsomath.cpp \
    optimist.cpp \
    aboutellipsofit.cpp \
    optiondialog.cpp \
    mplatfnt.cpp
HEADERS += mainwindow.h \
    xyscene.h \
    realzoom.h \
    graphicsview.h \
    paramedit.h \
    ellipsomath.h \
    optimist.h \
    aboutellipsofit.h \
    optiondialog.h \
    mplatfnt.h
FORMS += mainwindow.ui \
    paramedit.ui \
    optimist.ui \
    aboutellipsofit.ui \
    optiondialog.ui
OBJECTS_DIR = ./build/obj
MOC_DIR = ./build/moc

# UI_DIR = ./build/ui
RCC_DIR = ./build/rcc
RESOURCES += data.qrc
win32:RC_FILE = file.rc

# install
win32 { 
    target.path = ../binary
    sources.files = $$SOURCES \
        $$HEADERS \
        $$RESOURCES \
        $$FORMS \
        $$RC_FILE \
        rainbow.png \
        data-icon.png \
        monitor-icon.png \
        open-icon.png \
        opti-icon.png \
        PDF-icon.png \
        quit-icon.png \
        save-icon.png \
        about-icon.png \
        earth-icon.png \
        gear-icon.png \
        fit.pro
    sources.path = ../binary/sources/fit
    INSTALLS += target \
        sources
}
unix { 
    target.path = /usr/bin
    INSTALLS += target
}

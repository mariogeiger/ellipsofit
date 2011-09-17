# -------------------------------------------------
# Project created by QtCreator 2010-03-10T08:54:26
# -------------------------------------------------
QT += network

win32::TARGET = EllipsoFit
unix::TARGET = ellipsofit-fit
macx::TARGET = Ellipsofit

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
    mplatfnt.h \
    thinfilm.h

FORMS += mainwindow.ui \
    paramedit.ui \
    optimist.ui \
    aboutellipsofit.ui \
    optiondialog.ui

RESOURCES += data.qrc

win32:RC_FILE = file.rc
macx:ICON = rainbow.icns

# install
win32 { 
    target.path = ../binary

    INSTALLS += target
}
unix { 
    target.path = /usr/bin
    INSTALLS += target
}


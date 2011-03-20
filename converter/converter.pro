#-------------------------------------------------
#
# Project created by QtCreator 2010-03-10T08:56:05
#
#-------------------------------------------------

win32::TARGET = Converter
unix::TARGET = ellipsofit-converter

TEMPLATE = app

SOURCES += main.cpp


win32 {
RC_FILE = file.rc
}

# install

win32 {
target.path = ../binary

INSTALLS += target
}

unix {
target.path = /usr/bin
INSTALLS += target
}

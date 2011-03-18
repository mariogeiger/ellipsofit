#-------------------------------------------------
#
# Project created by QtCreator 2010-03-10T08:56:05
#
#-------------------------------------------------

win32::TARGET = Converter
unix::TARGET = ellipsofit-converter

TEMPLATE = app

SOURCES += main.cpp

OBJECTS_DIR = ./build/obj
MOC_DIR = ./build/moc
UI_DIR = ./build/ui
RCC_DIR = ./build/rcc

win32 {
RC_FILE = file.rc
}

# install

win32 {
target.path = ../binary
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS $$RC_FILE converter.pro
sources.path = ../binary/sources/converter
INSTALLS += target sources
}

unix {
target.path = /usr/bin
INSTALLS += target
}

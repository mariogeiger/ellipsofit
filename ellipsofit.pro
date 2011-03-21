TEMPLATE      = subdirs

SUBDIRS       = fit \
                converter








# install

win32 {
target.path = binary

dll.files = $$[QMAKE_LIBDIR_QT]/QtGui4.dll $$[QMAKE_LIBDIR_QT]/QtCore4.dll $$[QMAKE_LIBDIR_QT]/mingwm10.dll $$[QMAKE_LIBDIR_QT]/libgcc_s_dw2-1.dll
dll.path = dll

INSTALLS += target dll
}

unix {
menu.files = unix/ellipsofit.desktop
menu.path = /usr/share/applications

share.files = unix/rainbow.xpm
share.path = /share/ellipsofit

target.path = /usr/bin
INSTALLS += target menu share
}

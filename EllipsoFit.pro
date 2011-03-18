TEMPLATE      = subdirs

SUBDIRS       = fit \
				converter

unix {
OTHER_FILES   = configure \
                COPYING \
                gpl-3.0.txt \
                tarball.sh \
                unix/ellipsofit.desktop \
                unix/rainbow.xpm \
                addsource.sh \
                wintarball.sh
}




# install

win32 {
target.path = binary

sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS EllipsoFit.pro COPYING gpl-3.0.txt rainbow.ico
sources.path = binary/sources

dll.files = $$[QMAKE_LIBDIR_QT]/QtGui4.dll $$[QMAKE_LIBDIR_QT]/QtCore4.dll $$[QMAKE_LIBDIR_QT]/mingwm10.dll $$[QMAKE_LIBDIR_QT]/libgcc_s_dw2-1.dll
dll.path = dll

INSTALLS += target sources dll
}

unix {
menu.files = unix/ellipsofit.desktop
menu.path = /usr/share/applications

share.files = unix/rainbow.xpm
share.path = /share/ellipsofit

target.path = /usr/bin
INSTALLS += target menu share
}

/*

	Copyright (c) 2010 by Mario Geiger <mario.geiger@epfl.ch>

     ***************************************************************************
     *                                                                         *
     *   This program is free software; you can redistribute it and/or modify  *
     *   it under the terms of the GNU General Public License as published by  *
     *   the Free Software Foundation; either version 3 of the License, or     *
     *   (at your option) any later version.                                   *
     *                                                                         *
     ***************************************************************************
    */

#include "mainwindow.h"

#include <QtGui/QApplication>

#include <time.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("antigol");
    a.setOrganizationDomain("setup.weeb.ch");
    a.setApplicationName("EllipsoFit-Fit");
    a.setApplicationVersion("0.5.99");

    qsrand(time(0));

    MainWindow w;
    w.show();

    return a.exec();
}

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

#ifndef ABOUTELLIPSOFIT_H
#define ABOUTELLIPSOFIT_H

#include <QDialog>

namespace Ui {
    class AboutEllipsofit;
}

class AboutEllipsofit : public QDialog {
    Q_OBJECT
public:
    AboutEllipsofit(QWidget *parent = 0);
    ~AboutEllipsofit();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AboutEllipsofit *ui;
};

#endif // ABOUTELLIPSOFIT_H

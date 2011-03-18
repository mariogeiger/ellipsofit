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

#ifndef DOCK_H
#define DOCK_H

#include <QWidget>
#include "ellipsomath.h"

class QStandardItemModel;
class QItemSelectionModel;

namespace Ui {
    class Paramedit;
}

class Paramedit : public QWidget {
    Q_OBJECT
public:
    Paramedit(QWidget *parent = 0);
    ~Paramedit();
    QString nbrOfCharge() const;
    QString relaxTime() const;
    QString opticResistivity() const;

public slots:
    void setParameters(Parameters p);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::Paramedit *ui;

private slots:
    void on_pushButton_del_clicked();
    void on_pushButton_add_clicked();
    void readData();

signals:
    void parametersModified();
};

#endif // DOCK_H

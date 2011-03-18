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

#include "aboutellipsofit.h"
#include "ui_aboutellipsofit.h"

#include <QDateTime>

AboutEllipsofit::AboutEllipsofit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutEllipsofit)
{
    ui->setupUi(this);

	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(close()));

	QString text = ui->label_2->text();
	text.replace("--version--", qApp->applicationVersion());
	text.replace("--qtversion--", QT_VERSION_STR);
	text.replace("--date--", __DATE__);
	text.replace("--time--", __TIME__);
	ui->label_2->setText(text);

	setFixedSize(520, 270);
}

AboutEllipsofit::~AboutEllipsofit()
{
    delete ui;
}

void AboutEllipsofit::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

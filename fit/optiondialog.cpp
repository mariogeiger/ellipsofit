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

#include "optiondialog.h"
#include "ui_optiondialog.h"

#include <QColorDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QTimer>

OptionDialog::OptionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDialog)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(displayInfo()));
    timer->setInterval(3000);
    timer->setSingleShot(true);

    for (int st = 0; st < 2; ++st) {
        QGridLayout *l = new QGridLayout((st == 0) ? ui->tab : ui->tab_2);
        for (int i = 0; i < 7; ++i) {
            colorButtons[st][i] = new ColorButton(this);
            connect(colorButtons[st][i], SIGNAL(clicked()), this, SLOT(buttonColorClicked()));
            connect(colorButtons[st][i], SIGNAL(mouseMove()), this, SLOT(displayInfo()));
            l->addWidget(colorButtons[st][i], i / 3, i % 3);
        }
    }
}

OptionDialog::~OptionDialog()
{
    delete ui;
}

void OptionDialog::setPsiDelta(bool on)
{
    ui->radioButton->setChecked(on);
    ui->radioButton_2->setChecked(!on);
}

bool OptionDialog::psiDelta() const
{
    return ui->radioButton->isChecked();
}

void OptionDialog::setBlindZoom(bool on)
{
    ui->checkBox->setChecked(on);
}

bool OptionDialog::blindZoom() const
{
    return ui->checkBox->isChecked();
}

void OptionDialog::setColor(int st, int id, QColor c)
{
    colors[st][id] = c;
    colorButtons[st][id]->setStyleSheet(QString("background-color: %1").arg(c.name()));
}

QColor OptionDialog::color(int st, int id) const
{
    return colors[st][id];
}

void OptionDialog::buttonColorClicked()
{
    QPushButton *p = qobject_cast<QPushButton *>(sender());

    int st = -1;
    int id = -1;

    for (int s = 0; s < 2; ++s) {
        for (int i = 0; i < 7; ++i) {
            if (colorButtons[s][i] == p) {
                st = s;
                id = i;
            }
        }
    }

    if (id == -1)
        return;

    QColorDialog cd(colors[st][id]);

    if (cd.exec() == QDialog::Accepted) {
        setColor(st, id, cd.selectedColor());
        emit colorChanged();
    }
}

void OptionDialog::displayInfo()
{
    QPushButton *p = qobject_cast<QPushButton *>(sender());

//    int st = -1;
    int id = -1;

    for (int s = 0; s < 2; ++s) {
        for (int i = 0; i < 7; ++i) {
            if (colorButtons[s][i] == p) {
//                st = s;
                id = i;
            }
        }
    }

    switch (id) {
    case 0:
        ui->label->setText("functions color");
        break;
    case 1:
        ui->label->setText("scatter color");
        break;
    case 2:
        ui->label->setText("background color");
        break;
    case 3:
        ui->label->setText("axis color");
        break;
    case 4:
        ui->label->setText("horizontal lines color");
        break;
    case 5:
        ui->label->setText("text color");
        break;
    case 6:
        ui->label->setText("zoom color");
        break;
    default:
        ui->label->clear();
        break;
    }

    timer->start();
}



void OptionDialog::changeEvent(QEvent *e)
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

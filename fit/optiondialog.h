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

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
    class OptionDialog;
}

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    explicit ColorButton(QWidget * parent = 0)
        : QPushButton(parent)
    {
        this->setMouseTracking(true);
    }

signals:
    void mouseMove();

protected:
    virtual void mouseMoveEvent(QMouseEvent *)
    {
        emit mouseMove();
    }
};

class OptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionDialog(QWidget *parent = 0);
    ~OptionDialog();

    void setPsiDelta(bool);
    bool psiDelta() const;

    void setColor(int st, int id, QColor);
    QColor color(int st, int id) const;

    void setBlindZoom(bool);
    bool blindZoom() const;

signals:
    void colorChanged();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::OptionDialog *ui;
    QColor colors[2][7];
    ColorButton *colorButtons[2][7];

    QTimer *timer;

private slots:
    void buttonColorClicked();
    void displayInfo();
};

#endif // OPTIONDIALOG_H

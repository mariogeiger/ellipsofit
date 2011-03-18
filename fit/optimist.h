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

#ifndef OPTIMIST_H
#define OPTIMIST_H

#include <QWidget>
#include <QThread>
#include <QMutex>
#include "xyscene.h"
#include "ellipsomath.h"

class OptiThread;

namespace Ui {
    class Optimist;
}

class Optimist : public QWidget {
    Q_OBJECT
public:
    Optimist(XYScatterplot *rsp, XYScatterplot *isp, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~Optimist();

    static qreal coefficientOfDetermination(const QList<QPointF> &real, const QList<QPointF> &imag, const Parameters &para);

signals:
    void parameterFind(Parameters para);
    void optimizationRun(bool);

protected:
    void changeEvent(QEvent *e);

private:
    void startThreads();
    void stopThreads();

    Ui::Optimist *ui;
    XYScatterplot *m_rsp;
    XYScatterplot *m_isp;
    QList<OptiThread *> threads;

    QTimer *m_timer;

private slots:
    void on_pushButton_clicked();
    void refreshStats();
};

inline qreal random(qreal min, qreal max);

class OptiThread : public QThread {
    Q_OBJECT
public:
    OptiThread(XYScatterplot *rsp, XYScatterplot *isp, const Parameters &pref, const Parameters &pdel, bool qa, QObject * parent = 0);
    void stop();

    static Parameters getTheBestParameters();

    static Parameters s_bestpar;
    static qreal s_bstR2;
    static uint s_tests;
    static uint s_goods;


signals:
    void optfind();

protected:
    void run();

private:
    void makeMinMax(Parameters &pmin, Parameters &pmax);

    QList<QPointF> m_rsp;
    QList<QPointF> m_isp;
    Parameters m_pRef;
    Parameters m_pDel;
    bool m_qa;

    bool m_run;

    static QMutex s_mutex;
};

#endif // OPTIMIST_H

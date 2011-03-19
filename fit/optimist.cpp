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

#include "optimist.h"
#include "mplatfnt.h"
#include "ui_optimist.h"

#include <QMutexLocker>
#include <QSettings>

Optimist::Optimist(XYScatterplot *realEpsilonData, XYScatterplot *imaginaryEpsilonData, XYScatterplot *reflectivityData, QWidget *parent, Qt::WindowFlags f) :
        QWidget(parent, f),
        ui(new Ui::Optimist)
{
    ui->setupUi(this);

    m_realEpsilonData = realEpsilonData;
    m_imaginaryEpsilonData = imaginaryEpsilonData;
    m_reflectivityData = reflectivityData;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(refreshStats()));
    m_timer->setInterval(500);

    QSettings settings;
    ui->dsbI->setValue(settings.value("optimist_dsbi", 1.0).toReal());
    ui->dsbP->setValue(settings.value("optimist_dsbp", 1.0).toReal());
    ui->dsbG->setValue(settings.value("optimist_dsbg", 0.5).toReal());
    ui->dsbEk->setValue(settings.value("optimist_dsbek", 0.01).toReal());
    ui->dsbFk->setValue(settings.value("optimist_dsbfk", 0.04).toReal());
    ui->dsbGk->setValue(settings.value("optimist_dsbgk", 0.04).toReal());
    ui->qa->setChecked(settings.value("optimist_qa", true).toBool());
    ui->reflectivityfit->setChecked(settings.value("optimist_reflectivityfit", false).toBool());
}

Optimist::~Optimist()
{
    QSettings settings;
    settings.setValue("optimist_dsbi", ui->dsbI->value());
    settings.setValue("optimist_dsbp", ui->dsbP->value());
    settings.setValue("optimist_dsbg", ui->dsbG->value());
    settings.setValue("optimist_dsbek", ui->dsbEk->value());
    settings.setValue("optimist_dsbfk", ui->dsbFk->value());
    settings.setValue("optimist_dsbgk", ui->dsbGk->value());
    settings.setValue("optimist_qa", ui->qa->isChecked());
    settings.setValue("optimist_reflectivityfit", ui->reflectivityfit->isChecked());
    delete ui;
}

void Optimist::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

qreal Optimist::coefficientOfDeterminationOfEllipsometry(const QList<QPointF> &real, const QList<QPointF> &imag, const Parameters &para)
{
    Q_ASSERT(real.size() == imag.size());

    qreal n = real.size();
    qreal avgR = 0.0;
    qreal avgI = 0.0;

    for (int i = real.size() - 1; i >= 0; --i) {
        avgR += real[i].y();
        avgI += imag[i].y();
    }

    avgR /= n;
    avgI /= n;

    qreal sstotR = 0.0;
    qreal sserrR = 0.0;
    qreal sstotI = 0.0;
    qreal sserrI = 0.0;

    qreal tmp;
    for (int i = 0; i < real.size(); ++i) {
        tmp = real[i].y() - avgR;
        sstotR += tmp * tmp;

        tmp = real[i].y() - mathRealFun(real[i].x(), para);
        sserrR += tmp * tmp;

        tmp = imag[i].y() - avgI;
        sstotI += tmp * tmp;

        tmp = imag[i].y() - mathImagFun(imag[i].x(), para);
        sserrI += tmp * tmp;
    }

    return ((1.0 - sserrR / sstotR) + (1.0 - sserrI / sstotR)) / 2.0;
}

qreal Optimist::coefficientOfDeterminationOfReflectivity(const QList<QPointF> &reflectivity, const Parameters &para)
{
    qreal n = reflectivity.size();
    qreal average = 0.0;

    for (int i = reflectivity.size() - 1; i >= 0; --i) {
        average += reflectivity[i].y();
    }
    average /= n;

    qreal sstot = 0.0;
    qreal sserr = 0.0;

    qreal tmp;
    for (int i = reflectivity.size() - 1; i >= 0; --i) {
        tmp = reflectivity[i].y() - average;
        sstot += tmp * tmp;

        tmp = reflectivity[i].y() - mathReflectivity(reflectivity[i].x(), para);
        sserr += tmp * tmp;
    }

    return 1.0 - sserr / sstot;
}

void Optimist::on_pushButton_clicked()
{
    ui->tab_2->setDisabled(threads.isEmpty());
    ui->qa->setDisabled(threads.isEmpty());
    ui->reflectivityfit->setDisabled(threads.isEmpty());

    if (threads.isEmpty()) {
        ui->pushButton->setText(tr("Stop"));
        startThreads();
    } else {
        ui->pushButton->setText(tr("Start !"));
        stopThreads();
    }
}

void Optimist::refreshStats()
{
    static quint32 goods;
    ui->label_7->setText(tr("R<span style=\" vertical-align:super;\">2</span> = %1  (%L2/%L3)")
                         .arg(OptiThread::s_bstR2)
                         .arg(OptiThread::s_goods)
                         .arg(OptiThread::s_tests));

    if (goods != OptiThread::s_goods) {
        goods = OptiThread::s_goods;
        parameters = OptiThread::getTheBestParameters();
        emit parameterFind(parameters);
    }
}

void Optimist::startThreads()
{
    emit optimizationRun(true);

    OptiThread::s_bstR2 = Optimist::coefficientOfDeterminationOfEllipsometry(m_realEpsilonData->getPoints(), m_imaginaryEpsilonData->getPoints(), parameters);
    if (ui->reflectivityfit->isChecked() && !m_reflectivityData->isEmpty()) {
        OptiThread::s_bstR2 += Optimist::coefficientOfDeterminationOfReflectivity(m_reflectivityData->getPoints(), parameters);
        OptiThread::s_bstR2 /= 2.0;
    }

    ui->label_8->setText(tr("initial R<span style=\" vertical-align:super;\">2</span> = %1").arg(OptiThread::s_bstR2));

    OptiThread::s_bestpar = parameters;
    Parameters pDel;
    pDel.laurentians.append(Laurentian());

    pDel.einf = ui->dsbI->value();
    pDel.ep = ui->dsbP->value();
    pDel.g = ui->dsbG->value();
    pDel.laurentians[0].k[LaurEK] = ui->dsbEk->value();
    pDel.laurentians[0].k[LaurFK] = ui->dsbFk->value();
    pDel.laurentians[0].k[LaurGK] = ui->dsbGk->value();

    OptiThread::s_tests = 0;
    OptiThread::s_goods = 0;
    int cpu = ncpu();
    for (int i = 0; i < cpu; ++i) {
        OptiThread *thr = new OptiThread(m_realEpsilonData, m_imaginaryEpsilonData, m_reflectivityData, OptiThread::s_bestpar, pDel, ui->qa->isChecked(), ui->reflectivityfit->isChecked());
        thr->start();
        threads.append(thr);
    }
    m_timer->start();
}

void Optimist::stopThreads()
{
    m_timer->stop();
    for (int i = 0; i < threads.size(); ++i) {
        threads[i]->stop();
        threads[i]->wait();
        delete threads[i];
    }

    threads.clear();

    ui->label_7->clear();
    ui->label_8->setText(tr("final R<span style=\" vertical-align:super;\">2</span> = %1").arg(OptiThread::s_bstR2));

    parameters = OptiThread::getTheBestParameters();
    emit parameterFind(parameters);
    emit optimizationRun(false);
}


QMutex OptiThread::s_mutex;
Parameters OptiThread::s_bestpar;
qreal OptiThread::s_bstR2;
quint32 OptiThread::s_goods;
quint32 OptiThread::s_tests;


OptiThread::OptiThread(XYScatterplot *realEpsilon, XYScatterplot *imaginaryEpsilon, XYScatterplot *reflectivity, const Parameters &pref, const Parameters &pdel, bool qa, bool withReflectivity, QObject *parent) :
        QThread(parent)
{
    m_realEpsilon = realEpsilon->getPoints();
    m_imaginaryEpsilon = imaginaryEpsilon->getPoints();
    m_reflectivity = reflectivity->getPoints();

    m_pRef = pref;
    m_pDel = pdel;
    m_qa = qa;
    m_useReflexion = withReflectivity && !m_reflectivity.isEmpty();
}

Parameters OptiThread::getTheBestParameters()
{
    QMutexLocker ml(&s_mutex);
    return s_bestpar;
}

void OptiThread::stop()
{
    m_run = false;
}

void OptiThread::makeMinMax(Parameters &pmin, Parameters &pmax)
{
    pmin = pmax = m_pRef;

    pmin.einf = m_pRef.einf - m_pDel.einf;
    pmax.einf = m_pRef.einf + m_pDel.einf;

    pmin.ep = m_pRef.ep - m_pDel.ep;
    pmax.ep = m_pRef.ep + m_pDel.ep;

    pmin.g = m_pRef.g - m_pDel.g;
    pmax.g = m_pRef.g + m_pDel.g;

    for (int i = 0; i < m_pRef.laurentians.size(); ++i) {
        for (int k = 0; k < 3; ++k) {
            pmin.laurentians[i].k[k] = m_pRef.laurentians.at(i).k[k] - m_pDel.laurentians.at(0).k[k];
            pmax.laurentians[i].k[k] = m_pRef.laurentians.at(i).k[k] + m_pDel.laurentians.at(0).k[k];
        }
    }
}

void OptiThread::run()
{
    Parameters pCur, pMin, pMax;

    pCur = m_pRef;
    makeMinMax(pMin, pMax);

    quint32 goods = 0;

    m_run = true;

    while (m_run) {
        pCur.einf = random(pMin.einf, pMax.einf);
        pCur.ep = random(pMin.ep, pMax.ep);
        pCur.g = random(pMin.g, pMax.g);
        for (int i = 0; i < m_pRef.laurentians.size(); ++i) {
            for (int k = 0; k < 3; ++k)
                pCur.laurentians[i].k[k] = random(pMin.laurentians[i].k[k], pMax.laurentians[i].k[k]);
        }

        qreal r2 = Optimist::coefficientOfDeterminationOfEllipsometry(m_realEpsilon, m_imaginaryEpsilon, pCur);
        if (m_useReflexion) {
            r2 += Optimist::coefficientOfDeterminationOfReflectivity(m_reflectivity, pCur);
            r2 /= 2.0;
        }

        if (r2 > s_bstR2) {
            s_mutex.lock();
            ++s_goods;

            s_bstR2 = r2;
            s_bestpar = pCur;
            emit optfind();

            s_mutex.unlock();
        }

        if (m_qa && goods != s_goods) {
            goods = s_goods;

            s_mutex.lock();
            m_pRef = s_bestpar;
            s_mutex.unlock();

            makeMinMax(pMin, pMax);
        }

        ++s_tests;
    }
}


inline qreal random(qreal min, qreal max)
{
    if (max < 0.0)
        return 0.0;
    qreal result;
    do {
        qreal r = (qreal)qrand() / (qreal)RAND_MAX;
        qreal r2 = r * r;
        r = 4.0 * r2 * r - 6.0 * r2 + 3.0 * r;
        result = r * (max - min) + min;
    } while (result < 0.0);
    return result;
}

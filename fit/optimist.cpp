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

Optimist::Optimist(XYScatterplot *rsp, XYScatterplot *isp, QWidget *parent, Qt::WindowFlags f) :
		QWidget(parent, f),
		ui(new Ui::Optimist)
{
    ui->setupUi(this);

    m_rsp = rsp;
    m_isp = isp;

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

qreal Optimist::coefficientOfDetermination(const QList<QPointF> &real, const QList<QPointF> &imag, const Parameters &para)
{
    Q_ASSERT(real.size() == imag.size());

    qreal n = real.size();
    qreal moyR = 0.0;
    qreal moyI = 0.0;

    for (int i = 0; i < real.size(); ++i) {
		moyR += real[i].y() / n;
		moyI += imag[i].y() / n;
    }

    qreal sstotR = 0.0;
    qreal sserrR = 0.0;
    qreal sstotI = 0.0;
    qreal sserrI = 0.0;

    qreal tmp;
    for (int i = 0; i < real.size(); ++i) {
		tmp = real[i].y() - moyR;
		sstotR += tmp * tmp;

		tmp = real[i].y() - mathRealFun(real[i].x(), para);
		sserrR += tmp * tmp;

		tmp = imag[i].y() - moyI;
		sstotI += tmp * tmp;

		tmp = imag[i].y() - mathImagFun(imag[i].x(), para);
		sserrI += tmp * tmp;
    }

    return ((1.0 - sserrR / sstotR) + (1.0 - sserrI / sstotR)) / 2.0;
}

void Optimist::on_pushButton_clicked()
{
	ui->tab_2->setDisabled(threads.isEmpty());
	ui->qa->setDisabled(threads.isEmpty());

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

	OptiThread::s_bstR2 = Optimist::coefficientOfDetermination(m_rsp->getPoints(), m_isp->getPoints(), parameters);

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
		OptiThread *thr = new OptiThread(m_rsp, m_isp, OptiThread::s_bestpar, pDel, ui->qa->isChecked());
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


OptiThread::OptiThread(XYScatterplot *rsp, XYScatterplot *isp, const Parameters &pref, const Parameters &pdel, bool qa, QObject *parent) :
		QThread(parent)
{
    m_rsp = rsp->getPoints();
    m_isp = isp->getPoints();

	m_pRef = pref;
	m_pDel = pdel;
	m_qa = qa;
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

		qreal r2 = Optimist::coefficientOfDetermination(m_rsp, m_isp, pCur);

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

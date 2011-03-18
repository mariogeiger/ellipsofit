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

#include "paramedit.h"
#include "ui_paramedit.h"

#include <QStandardItemModel>

#include <math.h>

Paramedit::Paramedit(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::Paramedit)
{
	ui->setupUi(this);

	ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "Ek" << "fk" << "Gk");
	ui->tableWidget->sortByColumn(0, Qt::AscendingOrder);

	connect(ui->tableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(readData()));
	connect(ui->doubleSpinBox_einf, SIGNAL(valueChanged(double)), this, SLOT(readData()));
	connect(ui->doubleSpinBox_ep, SIGNAL(valueChanged(double)), this, SLOT(readData()));
	connect(ui->doubleSpinBox_g, SIGNAL(valueChanged(double)), this, SLOT(readData()));

	readData();
}

Paramedit::~Paramedit()
{
	delete ui;
}

void Paramedit::setParameters(Parameters p)
{
	ui->doubleSpinBox_einf->setValue(p.einf);
	ui->doubleSpinBox_ep->setValue(p.ep);
	ui->doubleSpinBox_g->setValue(p.g);

	while (ui->tableWidget->rowCount() != 0)	/* remove all the table */
		ui->tableWidget->removeRow(0);

	ui->tableWidget->setSortingEnabled(false);
	for (int i = p.laurentians.size() - 1; i >= 0; --i) {
		ui->tableWidget->insertRow(0);
		ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString::number(p.laurentians[i].k[LaurEK])));
		ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(p.laurentians[i].k[LaurFK])));
		ui->tableWidget->setItem(0, 2, new QTableWidgetItem(QString::number(p.laurentians[i].k[LaurGK])));
	}
	ui->tableWidget->setSortingEnabled(true);
}

void Paramedit::changeEvent(QEvent *e)
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

void Paramedit::on_pushButton_add_clicked()
{
	ui->tableWidget->setSortingEnabled(false);
	ui->tableWidget->insertRow(0);
	ui->tableWidget->setItem(0, 0, new QTableWidgetItem("0"));
	ui->tableWidget->setItem(0, 1, new QTableWidgetItem("1"));
	ui->tableWidget->setItem(0, 2, new QTableWidgetItem("1"));
	ui->tableWidget->setSortingEnabled(true);
}

void Paramedit::on_pushButton_del_clicked()
{
	QList<QTableWidgetSelectionRange> selection = ui->tableWidget->selectedRanges();
	if (!selection.isEmpty()) {
		ui->tableWidget->removeRow(selection.first().bottomRow());
		readData();
	}
}

void Paramedit::readData()
{
	parameters.einf = ui->doubleSpinBox_einf->value();
	parameters.ep = ui->doubleSpinBox_ep->value();
	parameters.g = ui->doubleSpinBox_g->value();

	parameters.laurentians.clear();

	for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
		bool ok;
		Laurentian laur = {{0.0, 0.0, 0.0}};

		for (int column = 0; column < ui->tableWidget->columnCount(); ++column) {

			QTableWidgetItem *item = ui->tableWidget->item(row, column);
			if (item == 0) {
				ok = false;
				break;
			}

			double value = item->data(Qt::DisplayRole).toDouble(&ok);

			if (!ok)
				break;

			laur.k[column] = value;
		}

		if (ok)
			parameters.laurentians.append(laur);
	}

	emit parametersModified();

	double chargenb = 7.26e20 * parameters.ep * parameters.ep;
	double relaxtime = 6.5815e-16 / parameters.g;
	double opticresistivity = 7435 * (parameters.g / (parameters.ep * parameters.ep));

	ui->label_n->setText(QString::number(chargenb));
	ui->label_t->setText(QString::number(relaxtime));
	ui->label_r->setText(QString::number(opticresistivity));
}

QString Paramedit::nbrOfCharge() const
{
	return ui->label_n->text();
}

QString Paramedit::relaxTime() const
{
	return ui->label_t->text();
}

QString Paramedit::opticResistivity() const
{
	return ui->label_r->text();
}

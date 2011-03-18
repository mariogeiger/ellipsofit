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

#include <QtGui/QApplication>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("antigol");
    a.setOrganizationDomain("setup.weeb.ch");
        a.setApplicationName("EllipsoFit-Converter");

    QSettings settings;

	QStringList infiles;
	if (a.arguments().size() >= 2) {
		infiles.append(a.arguments().at(1));
	} else {
		infiles = QFileDialog::getOpenFileNames(0, "Choose input files", settings.value("defaultdir", QDir::homePath()).toString());
	}

	if (infiles.isEmpty())
        return 0;

	settings.setValue("defaultdir", QFileInfo(infiles.first()).path());

	for (int i = 0; i < infiles.size(); ++i) {
		QString outfile;
		if (a.arguments().size() >= 2) {
			if (a.arguments().size() >= 3)
				outfile = a.arguments().at(2);
			else
				outfile = infiles[i] + ".std";
		} else {
			outfile = QFileDialog::getSaveFileName(0, "Choose an output file name for \"" + QFileInfo(infiles[i]).fileName() + "\"", infiles[i] + ".csv");
		}

		if (infiles[i] == outfile) {
			QMessageBox::critical(0, "Same files", "The output file cannot be the same as the input file.", QMessageBox::Retry);
			--i;
			continue;
		}

		QFile fin(infiles[i]);
		if (!fin.open(QIODevice::ReadOnly | QIODevice::Text)) {
			int answer = QMessageBox::critical(0, "Cannot open the input file !", fin.errorString(),
											   QMessageBox::Retry | QMessageBox::Ignore | QMessageBox::Abort);

			switch (answer) {
			case QMessageBox::Retry:
				--i;
				continue;
				break;
			case QMessageBox::Ignore:
				continue;
				break;
			case QMessageBox::Abort:
				return 0;
			default:
				break;
			}
		}

		QFile fout(outfile);
		if (!fout.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
			int answer = QMessageBox::critical(0, "Cannot create the output file !", fout.errorString(),
											   QMessageBox::Retry | QMessageBox::Ignore | QMessageBox::Abort);

			switch (answer) {
			case QMessageBox::Retry:
				--i;
				continue;
				break;
			case QMessageBox::Ignore:
				continue;
				break;
			case QMessageBox::Abort:
				return 0;
			default:
				break;
			}
		}

		QTextStream in(&fin);
		QTextStream out(&fout);
		QStringList linesSkiped;

		for (int line = 1; !in.atEnd(); ++line) {
			const QString strline = in.readLine();

			if (strline.contains("MINIMA"))
				break;

			QStringList numbers = strline.split(QRegExp("\\s"));
			if (numbers.size() < 6) {
				linesSkiped << QString::number(line);
				continue;
			}

			bool ok;

			const qreal energy = numbers[0].toDouble(&ok);
			if (!ok) {
				linesSkiped << QString::number(line);
				continue;
			}

			const qreal realValue = numbers[4].toDouble(&ok);
			if (!ok) {
				linesSkiped << QString::number(line);
				continue;
			}

			const qreal imaginaryValue = numbers[5].toDouble(&ok);
			if (!ok) {
				linesSkiped << QString::number(line);
				continue;
			}

			out << QString("%1;%2;%3").arg(energy).arg(realValue).arg(imaginaryValue) << endl;
		}

		if (linesSkiped.isEmpty()) {
			QMessageBox::information(0, "Done", "File well converted !");
		} else {
			QMessageBox::warning(0, "Done", QString("File converted !\nThese lines were skiped.\n%1").arg(linesSkiped.join(", ")));
		}
	}

    return 0;
}

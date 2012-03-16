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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ellipsomath.h"
#include "mplatfnt.h"
#include "aboutellipsofit.h"

#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QHttp>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressBar>
#include <QSettings>
#include <QStatusBar>
#include <QTextStream>
#include <QTime>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(qApp->applicationName() + " " + qApp->applicationVersion());

    QSettings settings;
    m_currentEllipsometryFile = settings.value("currentEllipsometryFile").toString();
    m_currentReflectivityFile = settings.value("currentReflectivityFile").toString();
    m_currentOpenFile = settings.value("currentOpenFile").toString();
    m_currentSaveFile = settings.value("currentSaveFile").toString();
    m_currentPrintFile = settings.value("currentPrintFile").toString();

    m_colors[0][0] = settings.value("xycolorstyle_00", Qt::blue).value<QColor>();
    m_colors[0][1] = settings.value("xycolorstyle_01", Qt::white).value<QColor>();
    m_colors[0][2] = settings.value("xycolorstyle_02", QColor(10, 10, 10)).value<QColor>();
    m_colors[0][3] = settings.value("xycolorstyle_03", Qt::white).value<QColor>();
    m_colors[0][4] = settings.value("xycolorstyle_04", QColor(25, 25, 25)).value<QColor>();
    m_colors[0][5] = settings.value("xycolorstyle_05", Qt::white).value<QColor>();
    m_colors[0][6] = settings.value("xycolorstyle_06", Qt::yellow).value<QColor>();

    m_colors[1][0] = settings.value("xycolorstyle_10", Qt::blue).value<QColor>();
    m_colors[1][1] = settings.value("xycolorstyle_11", Qt::red).value<QColor>();
    m_colors[1][2] = settings.value("xycolorstyle_12", Qt::white).value<QColor>();
    m_colors[1][3] = settings.value("xycolorstyle_13", Qt::black).value<QColor>();
    m_colors[1][4] = settings.value("xycolorstyle_14", Qt::lightGray).value<QColor>();
    m_colors[1][5] = settings.value("xycolorstyle_15", Qt::black).value<QColor>();
    m_colors[1][6] = settings.value("xycolorstyle_16", Qt::black).value<QColor>();

    QRect r = settings.value("geometry", QRect(0, 0, 0, 0)).toRect();
    if (!r.isNull())
        setGeometry(r);
    if (settings.value("fullscreen", false).toBool()) {
        setWindowState(windowState() | Qt::WindowFullScreen);
        ui->action_Fullscreen->setChecked(true);
    }

    m_sceneReal = new XYScene(this);
    m_sceneImag = new XYScene(this);
    m_sceneRefl = new XYScene(this);

    ui->graphicsViewReal->setScene(m_sceneReal);
    ui->graphicsViewImag->setScene(m_sceneImag);
    ui->graphicsViewRefl->setScene(m_sceneRefl);

    m_sceneReal->setState(m_sceneReal->state() | XYScene::SendMouseMove);
    m_sceneImag->setState(m_sceneImag->state() | XYScene::SendMouseMove);
    m_sceneRefl->setState(m_sceneRefl->state() | XYScene::SendMouseMove);
    if (settings.value("horizontal_zoom_bind", false).toBool()) {
        m_sceneReal->setState(m_sceneReal->state() | XYScene::SendZoomChanged);
        m_sceneImag->setState(m_sceneImag->state() | XYScene::SendZoomChanged);
        m_sceneRefl->setState(m_sceneRefl->state() | XYScene::SendZoomChanged);
    }

    m_sceneReal->setZoom(RealZoom(0.0, 6.5, -11.0, 11.0));
    m_sceneImag->setZoom(RealZoom(0.0, 6.5, 0.0, 22.0));
    m_sceneRefl->setZoom(RealZoom(0.0, 6.5, 0.0, 1.0));

    m_dataReal = new XYScatterplot();
    m_dataImag = new XYScatterplot();
    m_dataRefl = new XYScatterplot();

    m_funcReal = new XYRealFun(parameters);
    m_drudReal = new XYRealDrud(parameters);
    m_funcImag = new XYImagFun(parameters);
    m_drudImag = new XYImagDrud(parameters);
    m_funcRefl = new XYReflFun(parameters);

    m_sceneReal->appendScatterplot(m_dataReal);
    m_sceneReal->appendFunction(m_drudReal); // draw in first the little
    m_sceneReal->appendFunction(m_funcReal);

    m_sceneImag->appendScatterplot(m_dataImag);
    m_sceneImag->appendFunction(m_drudImag);
    m_sceneImag->appendFunction(m_funcImag);

    m_sceneRefl->appendScatterplot(m_dataRefl);
    m_sceneRefl->appendFunction(m_funcRefl);

    setGraphicStyle();

    {
        m_dockParamedit = new QDockWidget(tr("Parameters"), this);
        m_dockParamedit->setFeatures(QDockWidget::DockWidgetMovable |
                                     QDockWidget::DockWidgetFloatable);
        m_dockParamedit->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        m_paramedit = new Paramedit(this);
        m_dockParamedit->setWidget(m_paramedit);
        //        ui->layoutfordocks->addWidget(m_paramedit);

        char pos = settings.value("paramedit_pos", 'l').toInt();
        switch (pos) {
        case 'r':
            addDockWidget(Qt::RightDockWidgetArea, m_dockParamedit);
            break;
        case 'l':
            addDockWidget(Qt::LeftDockWidgetArea, m_dockParamedit);
            break;
        case 'f':
            addDockWidget(Qt::RightDockWidgetArea, m_dockParamedit);
            m_dockParamedit->setFloating(true);
            break;
        }
        m_dockParamedit->setGeometry(settings.value("paramedit_geo", QRect(10, 10, 50, 100)).toRect());
    }

    {
        m_dockOptimist = new QDockWidget(tr("Optimist"), this);
        bool vis = settings.value("optimist_vis", true).toBool();
        m_dockOptimist->setVisible(vis);
        ui->actionOptimist_dock->setChecked(vis);
        m_dockOptimist->setFeatures(QDockWidget::DockWidgetMovable |
                                    QDockWidget::DockWidgetFloatable |
                                    QDockWidget::DockWidgetClosable);
        m_dockOptimist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

        m_optimist = new Optimist(m_dataReal, m_dataImag, m_dataRefl, this);
        m_dockOptimist->setWidget(m_optimist);
        //        ui->layoutfordocks->addWidget(m_optimist);

        char pos = settings.value("optimist_pos", 'r').toInt();
        switch (pos) {
        case 'r':
            addDockWidget(Qt::RightDockWidgetArea, m_dockOptimist);
            break;
        case 'l':
            addDockWidget(Qt::LeftDockWidgetArea, m_dockOptimist);
            break;
        case 'f':
            addDockWidget(Qt::RightDockWidgetArea, m_dockOptimist);
            m_dockOptimist->setFloating(true);
            break;
        }
        m_dockOptimist->setGeometry(settings.value("optimist_geo", QRect(10, 10, 50, 100)).toRect());
    }

    m_http = 0;
    QTime lastCheck = settings.value("last_check_update", 0).toTime();
    qDebug() << lastCheck.secsTo(QTime::currentTime());
    if (lastCheck.secsTo(QTime::currentTime()) > 12 * 3600)
        checkUpdate(false);


    psidelta = settings.value("psidelta_data", false).toBool();

    connexions();
}

MainWindow::~MainWindow()
{
    QSettings settings;
    {
        settings.setValue("currentEllipsometryFile", m_currentEllipsometryFile);
        settings.setValue("currentReflectivityFile", m_currentReflectivityFile);
        settings.setValue("currentOpenFile", m_currentOpenFile);
        settings.setValue("currentSaveFile", m_currentSaveFile);
        settings.setValue("currentPrintFile", m_currentPrintFile);
    }

    {
        bool fullscreen = windowState() & Qt::WindowFullScreen;
        settings.setValue("fullscreen", fullscreen);

        if (!fullscreen)
            settings.setValue("geometry", geometry());
    }

    {
        char pos = 'f';
        if (!m_dockParamedit->isFloating()) {
            if (dockWidgetArea(m_dockParamedit) == Qt::RightDockWidgetArea)
                pos = 'r';
            else
                pos = 'l';
        }
        settings.setValue("paramedit_pos", pos);
        settings.setValue("paramedit_geo", m_dockParamedit->geometry());
    }

    {
        char pos = 'f';
        if (!m_dockOptimist->isFloating()) {
            if (dockWidgetArea(m_dockOptimist) == Qt::RightDockWidgetArea)
                pos = 'r';
            else
                pos = 'l';
        }
        settings.setValue("optimist_pos", pos);
        settings.setValue("optimist_geo", m_dockOptimist->geometry());
        settings.setValue("optimist_vis", m_dockOptimist->isVisibleTo(this));
    }

    delete m_optimist;
    delete ui;
}

void MainWindow::checkUpdate(bool verbose)
{
    if (m_http != 0) {
        return;
    }
    m_http = new QHttp(this);
    m_http->setHost("setup.weeb.ch");
    if (verbose)
        connect(m_http, SIGNAL(requestFinished(int,bool)), this, SLOT(httpRequestFinishedVerbose(int,bool)));
    else
        connect(m_http, SIGNAL(requestFinished(int,bool)), this, SLOT(httpRequestFinishedSilent(int,bool)));

    QString str = QString::fromUtf8("http://setup.weeb.ch/update/ellipsofit.php?version=%1").arg(
                qApp->applicationVersion().remove('.').toInt());


    QBuffer *buffer = new QBuffer(&m_bufferData, m_http);
    if (buffer->open(QIODevice::ReadWrite)) {
        m_httpGetId = m_http->get(str, buffer);
        qDebug() << str;
    }
}

bool MainWindow::httpRequestFinishedSilent(int requestId, bool error)
{
    bool u2d = true;
    if (m_httpGetId == requestId && !error) {
        if (m_bufferData.startsWith("<!-- false -->")) {
            u2d = false;
            if (QMessageBox::question(this, tr("Update"), tr("A new version of Ellipsofit is available on the website.\n"
                                                             "Do you want to download it now ?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
                QDesktopServices::openUrl(QUrl(QString("http://setup.weeb.ch/update/ellipsofit.php")));
                qApp->quit();
            }
            qDebug() << "set new time";
            QSettings().setValue("last_check_update", QTime::currentTime());
        }
        m_http->deleteLater(); m_http = 0;
    }
    return u2d;
}

void MainWindow::httpRequestFinishedVerbose(int requestId, bool error)
{
    if (m_httpGetId == requestId) {
        if (error) {
            QMessageBox::warning(this, tr("Update error"), m_http->errorString());
        } else {
            if (httpRequestFinishedSilent(requestId, error))
                QMessageBox::information(this, tr("Update"), tr("You have the last version !"));
        }
        if (m_http)
            m_http->deleteLater(); m_http = 0;
    }
}

bool MainWindow::loadEllipsometryData(const QString &file)
{
    QFile ifile(file);
    if (!ifile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Cannot open the file"), tr("<em>%1</em><br />\n%2").arg(file).arg(ifile.errorString()));
        return false;
    }

    int goodline = 0;
    QString warnings;
    QTextStream in(&ifile);

    m_dataReal->clear();
    m_dataImag->clear();
    for (int line = 1; !in.atEnd(); ++line) {
        QPointF real;
        QPointF imag;
        bool ok;

        QStringList split = in.readLine().trimmed().split(QRegExp("[\\s;,:]"));

        if (split.size() < 3) {
            warnings.append(tr("line %1 : line too short<br />\n").arg(line));
            continue;
        }

        real.setX(split[0].toDouble(&ok));
        imag.setX(real.x());
        if (!ok) {
            warnings.append(tr("line %1 : cannot read the first column<br />\n").arg(line));
            continue;
        }

        if (psidelta) {

        } else {
            real.setY(split[1].toDouble(&ok));
        }
        if (!ok) {
            warnings.append(tr("line %1 : cannot read the second column<br />\n").arg(line));
            continue;
        }

        if (psidelta) {

        } else {
            imag.setY(split[2].toDouble(&ok));
        }
        if (!ok) {
            warnings.append(tr("line %1 : cannot read the third column<br />\n").arg(line));
            continue;
        }

        m_dataReal->append(real);
        m_dataImag->append(imag);
        ++goodline;
    }

    if (!warnings.isEmpty()) {
        QMessageBox::warning(this, tr("Warnings"), tr("<em>%1</em><br />"
                                                      "<strong>Warnings :</strong><br />"
                                                      "%2<br /><strong>%3 accepted lines.</strong>").arg(file).arg(warnings.left(1000)).arg(goodline));
    }

    m_sceneReal->regraph();
    m_sceneImag->regraph();

    ui->labelfilereal->setText(file);
    ui->labelfileimaginary->setText(file);

    return true;
}

bool MainWindow::loadReflectivityData(const QString &file)
{
    QFile ifile(file);
    if (!ifile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Cannot open the file"), tr("<em>%1</em><br />\n%2").arg(file).arg(ifile.errorString()));
        return false;
    }

    int goodline = 0;
    QString warnings;
    QTextStream in(&ifile);

    m_dataRefl->clear();
    for (int line = 1; !in.atEnd(); ++line) {
        QPointF reflectivity;
        bool ok;

        QStringList split = in.readLine().trimmed().split(QRegExp("[\\s;,:]"));

        if (split.size() < 2) {
            warnings.append(tr("line %1 : line too short<br />\n").arg(line));
            continue;
        }

        reflectivity.setX(split[0].toDouble(&ok));
        if (!ok) {
            warnings.append(tr("line %1 : cannot read the first column<br />\n").arg(line));
            continue;
        }

        reflectivity.setY(split[1].toDouble(&ok));
        if (!ok) {
            warnings.append(tr("line %1 : cannot read the second column<br />\n").arg(line));
            continue;
        }

        m_dataRefl->append(reflectivity);
        ++goodline;
    }

    if (!warnings.isEmpty()) {
        QMessageBox::warning(this, tr("Warnings"), tr("<em>%1</em><br />"
                                                      "<strong>Warnings :</strong><br />"
                                                      "%2<br /><strong>%3 accepted lines.</strong>").arg(file).arg(warnings.left(1000)).arg(goodline));
    }

    m_sceneRefl->regraph();

    ui->labelfilereflectivity->setText(file);

    return true;
}

bool MainWindow::saveResults(const QString &file)
{
    QFile ofile(file);
    if (!ofile.open(QIODevice::Text | QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Cannot create the file"), tr("<em>%1</em><br />\n%2").arg(file).arg(ofile.errorString()));
        return false;
    }

    bool ok;
    int nsave = QInputDialog::getInt(this, tr("Number of points"), tr("Choose how many points save."), 100, 0, 100000, 1, &ok);
    if (!ok) return false;

    QTextStream out(&ofile);

    out << "# ----------------------------------------------" << endl;
    out << QString("# File created by ellipsoFit %1")
           .arg(QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss")) << endl;
    out << "# ----------------------------------------------" << endl;

    if (QFileInfo(m_currentEllipsometryFile).isFile())
        out << "Ellipsometry file =\t" << m_currentEllipsometryFile << endl;

    if (QFileInfo(m_currentReflectivityFile).isFile())
        out << "Reflectivity file =\t" << m_currentReflectivityFile << endl;

    out << "Einf =\t" << QString::number(parameters.einf, 'g', 5) << endl;
    out << "Ep   =\t" << QString::number(parameters.ep, 'g', 5) << endl;
    out << "G    =\t" << QString::number(parameters.g, 'g', 5) << endl;
    for (int i = 0; i < parameters.laurentians.size(); ++i) {
        out << "Ek("<<i+1<<") =\t" << QString::number(parameters.laurentians[i].k[LaurEK], 'g', 5) << endl;
        out << "fk("<<i+1<<") =\t" << QString::number(parameters.laurentians[i].k[LaurFK], 'g', 5) << endl;
        out << "Gk("<<i+1<<") =\t" << QString::number(parameters.laurentians[i].k[LaurGK], 'g', 5) << endl;
    }
    out << endl;
    out << "N   =\t" << m_paramedit->nbrOfCharge() << " [cm-3]" << endl;
    out << "Tr  =\t" << m_paramedit->relaxTime() << " [s]" << endl;
    out << "Rop =\t" << m_paramedit->opticResistivity() << " [uohm cm]" << endl;
    out << endl;
    out << QString("%1 points of the fit :").arg(nsave) << endl;
    out << "Energy     \tReal value  \tImaginary value  \tReflectivity value" << endl;
    qreal x0 = qMin(m_sceneReal->zoom().xMin(), m_sceneImag->zoom().xMin());
    qreal delta = (qMax(m_sceneReal->zoom().xMax(), m_sceneImag->zoom().xMax()) - x0) / (qreal)nsave;
    for (int i = 0; i < nsave; ++i) {
        qreal x = x0 + delta * i;
        if (!m_funcReal->domain(x)) continue;
        const qreal yr = m_funcReal->y(x);
        const qreal yi = m_funcImag->y(x);
        const qreal yrefl = m_funcRefl->y(x);
        out << QString::number(x, 'e', 5) << '\t' <<
               QString::number(yr, 'e', 5) << '\t' <<
               QString::number(yi, 'e', 5) << '\t' <<
               QString::number(yrefl, 'e', 5) << endl;
    }
    QMessageBox::information(this, tr("Well save"), tr("Parameters and functions saved !"));

    setWindowTitle(qApp->applicationName() + " " + file);

    return true;
}

bool MainWindow::openResults(const QString &file)
{
    QFile ifile(file);
    if (!ifile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Cannot read the file"), tr("<em>%1</em><br />\n%2").arg(file).arg(ifile.errorString()));
        return false;
    }

    QTextStream in(&ifile);
    Parameters p;

    QString ellipsometryfile;
    QString reflectivityfile;
    Laurentian laur = {{0.0, 0.0, 0.0}};
    int laurArg;
    for (int linenumber = 1; !in.atEnd(); ++linenumber) {
        bool ok = true;

        laurArg = -1;
        QString line = in.readLine().simplified();

        // Data file was the notation v < 0.6.00
        if (line.startsWith("Ellipsometry file ") || line.startsWith("Data file")) {
            ellipsometryfile = line.section("=", 1).trimmed();
            QFileInfo fi(ellipsometryfile);
            if (!fi.exists()) {
                fi.setFile(QFileInfo(file).path(), fi.fileName());
                if (fi.exists())
                    ellipsometryfile = fi.filePath();
            }
        }

        if (line.startsWith("Reflectivity file ")) {
            reflectivityfile = line.section("=", 1).trimmed();
            QFileInfo fi(reflectivityfile);
            if (!fi.exists()) {
                fi.setFile(QFileInfo(file).path(), fi.fileName());
                if (fi.exists())
                    reflectivityfile = fi.filePath();
            }
        }

        if (line.startsWith("Einf ")) {
            p.einf = line.section("=", 1).toDouble(&ok);
        }
        if (line.startsWith("Ep ")) {
            p.ep = line.section("=", 1).toDouble(&ok);
        }
        if (line.startsWith("G ")) {
            p.g = line.section("=", 1).toDouble(&ok);
        }

        if (line.startsWith("Ek("))
            laurArg = LaurEK;
        if (line.startsWith("fk("))
            laurArg = LaurFK;
        if (line.startsWith("Gk("))
            laurArg = LaurGK;

        if (laurArg != -1) {
            int id = line.section("(", 1).section(")", 0, 0).toInt(&ok) - 1;

            if (ok) {
                while (p.laurentians.size() <= id)
                    p.laurentians.append(laur);

                double num = line.section("=", 1).toDouble(&ok);
                switch (laurArg) {
                case LaurEK:
                    p.laurentians[id].k[LaurEK] = num;
                    break;
                case LaurFK:
                    p.laurentians[id].k[LaurFK] = num;
                    break;
                case LaurGK:
                    p.laurentians[id].k[LaurGK] = num;
                    break;
                }
            }
        }

        if (!ok) {
            QMessageBox::critical(this, tr("Reading error"),
                                  tr("A digital conversion error is generated on line %1.\n"
                                     "Reading the document is interrupted !").arg(linenumber));
            return false;
        }
    }

    m_paramedit->setParameters(p);

    if (!ellipsometryfile.isEmpty())
        if(loadEllipsometryData(ellipsometryfile))
            m_currentEllipsometryFile = ellipsometryfile;

    if (!reflectivityfile.isEmpty())
        if (loadReflectivityData(reflectivityfile))
            m_currentReflectivityFile = reflectivityfile;

    setWindowTitle(qApp->applicationName() + " " + file);

    return true;
}

void MainWindow::synchroCurrentFiles()
{
    QString currentDir = QDir::homePath();
    QFileInfo dataFile(m_currentEllipsometryFile);
    QFileInfo reflFile(m_currentReflectivityFile);
    QFileInfo openFile(m_currentOpenFile);
    QFileInfo saveFile(m_currentSaveFile);
    QFileInfo printFile(m_currentPrintFile);

    if (dataFile.isFile()) {
        currentDir = dataFile.path();
    }
    if (reflFile.isFile()) {
        currentDir = reflFile.path();
    }
    if (openFile.isFile()) {
        currentDir = openFile.path();
    }
    if (saveFile.isFile()) {
        currentDir = saveFile.path();
    }
    if (printFile.isFile()) {
        currentDir = printFile.path();
    }

    if (!dataFile.isFile()) {
        m_currentEllipsometryFile = currentDir;
    }
    if (!reflFile.isFile()) {
        m_currentReflectivityFile = currentDir;
    }
    if (!openFile.isFile()) {
        m_currentOpenFile = currentDir;
    }
    if (!saveFile.isFile()) {
        m_currentSaveFile = currentDir;
    }
    if (!printFile.isFile()) {
        m_currentPrintFile = currentDir;
    }
}

void MainWindow::connexions()
{
    connect(ui->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(m_paramedit, SIGNAL(parametersModified()), this, SLOT(functionGestion()));
    connect(m_sceneReal, SIGNAL(mouseClic(QPointF,Qt::MouseButtons)), this, SLOT(xymouseClic(QPointF,Qt::MouseButtons)));
    connect(m_sceneImag, SIGNAL(mouseClic(QPointF,Qt::MouseButtons)), this, SLOT(xymouseClic(QPointF,Qt::MouseButtons)));

    connect(m_sceneReal, SIGNAL(zoomChanged()), this, SLOT(xysamexzoom()));
    connect(m_sceneImag, SIGNAL(zoomChanged()), this, SLOT(xysamexzoom()));
    connect(m_sceneRefl, SIGNAL(zoomChanged()), this, SLOT(xysamexzoom()));

    connect(m_dockOptimist, SIGNAL(visibilityChanged(bool)), ui->actionOptimist_dock, SLOT(setChecked(bool)));
    connect(m_optimist, SIGNAL(parameterFind(Parameters)), m_sceneReal, SLOT(regraph()));
    connect(m_optimist, SIGNAL(parameterFind(Parameters)), m_sceneImag, SLOT(regraph()));
    connect(m_optimist, SIGNAL(parameterFind(Parameters)), m_sceneRefl, SLOT(regraph()));
    connect(m_optimist, SIGNAL(parameterFind(Parameters)), m_paramedit, SLOT(setParameters(Parameters)));

    connect(m_optimist, SIGNAL(optimizationRun(bool)), this, SLOT(optimizationChangeState(bool)));
}

void MainWindow::setGraphicStyle(int st)
{
    m_st = st;

    m_sceneReal->setBackgroundBrush(QBrush(m_colors[st][2]));

    m_sceneReal->setSubaxesPen(QPen(m_colors[st][4]));
    m_sceneReal->setTextColor(m_colors[st][5]);
    m_sceneReal->setAxesPen(QPen(m_colors[st][3]));
    m_sceneReal->setZoomPen(QPen(m_colors[st][6]));
    m_sceneImag->setLook(m_sceneReal->look());
    m_sceneRefl->setLook(m_sceneReal->look());


    m_dataReal->setPen(QPen(m_colors[st][1]));
    m_dataReal->setBrush(Qt::NoBrush);
    m_dataReal->setRadius(1.5);

    m_dataImag->setPen(QPen(m_colors[st][1]));
    m_dataImag->setBrush(Qt::NoBrush);
    m_dataImag->setRadius(1.5);

    m_dataRefl->setPen(QPen(m_colors[st][1]));
    m_dataRefl->setBrush(Qt::NoBrush);
    m_dataRefl->setRadius(1.5);


    m_funcReal->setPen(QPen(QBrush(m_colors[st][0]), 2));
    m_drudReal->setPen(QPen(m_colors[st][0].light(130)));

    m_funcImag->setPen(QPen(QBrush(m_colors[st][0]), 2));
    m_drudImag->setPen(QPen(m_colors[st][0].light(130)));

    m_funcRefl->setPen(QPen(QBrush(m_colors[st][0]), 2));
}

void MainWindow::optimizationChangeState(bool run)
{
    m_paramedit->setDisabled(run);
    if (run) {
        m_sceneReal->setState(m_sceneReal->state() & ~XYScene::SendMouseMove);
        m_sceneImag->setState(m_sceneReal->state() & ~XYScene::SendMouseMove);
    } else {
        m_sceneReal->setState(m_sceneReal->state() | XYScene::SendMouseMove);
        m_sceneImag->setState(m_sceneReal->state() | XYScene::SendMouseMove);
    }
}

void MainWindow::xymouseClic(QPointF rpos, Qt::MouseButtons but)
{
    if (but & Qt::LeftButton) {
        Laurentian l;
        l.k[LaurEK] = rpos.x();
        l.k[LaurFK] = 1.0;
        l.k[LaurGK] = 0.5;
        parameters.laurentians.append(l);
        m_paramedit->setParameters(parameters);
    }
}

void MainWindow::functionGestion()
{
    // CLEARING
    m_laurentiansReal.clear();
    m_laurentiansImag.clear();
    for (int i = 0; i < m_sceneReal->getFunctionsList().size(); ++i) {
        XYFunction *fun = m_sceneReal->getFunctionsList().at(i);
        if (fun != m_funcReal && fun != m_drudReal) {
            m_sceneReal->getFunctionsList().removeAt(i);
            delete fun;
            --i;
        }
    }
    for (int i = 0; i < m_sceneImag->getFunctionsList().size(); ++i) {
        XYFunction *fun = m_sceneImag->getFunctionsList().at(i);
        if (fun != m_funcImag && fun != m_drudImag) {
            m_sceneImag->getFunctionsList().removeAt(i);
            delete fun;
            --i;
        }
    }

    for (int i = 0; i < parameters.laurentians.size(); ++i) {
        m_laurentiansReal.append(new XYRealLaurentian(parameters, QPen(m_colors[m_st][0].light(175))));
        m_laurentiansReal[i]->setNo(i);
        m_sceneReal->getFunctionsList().insert(0, m_laurentiansReal[i]);

        m_laurentiansImag.append(new XYImagLaurentian(parameters, QPen(m_colors[m_st][0].light(175))));
        m_laurentiansImag[i]->setNo(i);
        m_sceneImag->getFunctionsList().insert(0, m_laurentiansImag[i]);
    }

    m_sceneReal->regraph();
    m_sceneImag->regraph();
    m_sceneRefl->regraph();
}

void MainWindow::xysamexzoom()
{
    m_sceneImag->setState(m_sceneImag->state() & ~XYScene::SendZoomChanged);
    m_sceneReal->setState(m_sceneReal->state() & ~XYScene::SendZoomChanged);
    m_sceneRefl->setState(m_sceneRefl->state() & ~XYScene::SendZoomChanged);

    XYScene *scenes[3] = {m_sceneReal, m_sceneImag, m_sceneRefl};

    for (int i = 0; i < 3; ++i) {
        if (sender() == scenes[i]) {
            for (int j = 0; j < 3; ++j) {
                if (j != i) {
                    RealZoom z = scenes[j]->zoom();
                    z.setXMin(scenes[i]->zoom().xMin());
                    z.setXMax(scenes[i]->zoom().xMax());

                    scenes[j]->setZoom(z);
                    scenes[j]->regraph();
                }
            }
        }
    }

    m_sceneReal->setState(m_sceneReal->state() | XYScene::SendZoomChanged);
    m_sceneImag->setState(m_sceneImag->state() | XYScene::SendZoomChanged);
    m_sceneRefl->setState(m_sceneRefl->state() | XYScene::SendZoomChanged);
}



void MainWindow::on_actionLoad_data_triggered()
{
    synchroCurrentFiles();
    const QString file = QFileDialog::getOpenFileName(this, tr("Open ellipsometry file"), m_currentEllipsometryFile);
    if (file.isEmpty())
        return;

    if (loadEllipsometryData(file)) {
        QFileInfo fileinfo(file);
        m_currentEllipsometryFile = fileinfo.filePath();
        m_currentSaveFile = QFileInfo(fileinfo.path(), fileinfo.baseName().append(".txt")).filePath();
        m_currentPrintFile = QFileInfo(fileinfo.path(), fileinfo.baseName().append(".pdf")).filePath();
    }
}

void MainWindow::on_actionLoad_reflectivity_file_triggered()
{
    synchroCurrentFiles();
    const QString file = QFileDialog::getOpenFileName(this, tr("Open reflectivity file"), m_currentReflectivityFile);
    if (file.isEmpty())
        return;

    if (loadReflectivityData(file)) {
        QFileInfo fileinfo(file);
        m_currentReflectivityFile = fileinfo.filePath();
    }
}

void MainWindow::on_actionClear_reflectivity_data_triggered()
{
    m_dataRefl->clear();
    m_sceneRefl->regraph();
    ui->labelfilereflectivity->clear();
    m_currentReflectivityFile.clear();
}

void MainWindow::on_action_Save_results_triggered()
{
    synchroCurrentFiles();
    const QString file = QFileDialog::getSaveFileName(this, tr("Choose file name"), m_currentSaveFile);
    if (file.isEmpty())
        return;

    if (saveResults(file))
        m_currentSaveFile = file;
}

void MainWindow::on_actionOpen_results_file_triggered()
{
    synchroCurrentFiles();
    const QString file = QFileDialog::getOpenFileName(this, tr("Open a results file"), m_currentOpenFile);
    if (file.isEmpty())
        return;

    if (openResults(file)) {
        QFileInfo fileinfo(file);
        m_currentSaveFile = m_currentOpenFile = fileinfo.filePath();
        m_currentPrintFile = QFileInfo(fileinfo.path(), fileinfo.baseName().append(".pdf")).filePath();
    }
}

void MainWindow::on_action_Fullscreen_triggered(bool checked)
{
    if (checked) {
        QSettings().setValue("geometry", geometry());
        setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void MainWindow::on_action_Print_triggered()
{
    QPrinter printer(QPrinter::ScreenResolution);

    synchroCurrentFiles();
    const QString file = QFileDialog::getSaveFileName(this, tr("Print as PDF"), m_currentPrintFile, "PDF (*.pdf)");
    if (file.isEmpty())
        return;
    m_currentPrintFile = file;

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(file);
    printer.setPageSize(QPrinter::A4);


    QPainter painter(&printer);

    setGraphicStyle(1);
    functionGestion();

    int y = 0;
    int w = printer.width();

    painter.drawText(QRect(0, y, w, 25), Qt::AlignCenter, QFileInfo(file).baseName());
    painter.drawText(QRect(0, y, w, 25), Qt::AlignRight, QDate::currentDate().toString("dd MMMM yyyy"));
    y += 50;

    painter.drawText(0, y, "REAL");
    y += 10;

    QRect realRect(0, y, printer.width(), printer.height() / 3.5);
    m_sceneReal->setSceneRect(realRect);
    m_sceneReal->render(&painter, realRect);
    y += realRect.height() + 40;

    painter.drawText(0, y, "IMAGINARY");
    y += 10;

    QRect imagRect(0, y, printer.width(), printer.height() / 3.5);
    m_sceneImag->setSceneRect(imagRect);
    m_sceneImag->render(&painter, imagRect);
    y+= imagRect.height() + 40;

    QString text = QString("Einf = %1\nEp = %2\nG = %3\n")
            .arg(parameters.einf, 0, 'g', 5).arg(parameters.ep, 0, 'g', 5).arg(parameters.g, 0, 'g', 5);

    for (int i = 0; i < parameters.laurentians.size(); ++i) {
        text += QString("Ek(%1) = %2, ").arg(i+1)
                .arg(parameters.laurentians[i].k[LaurEK], 0, 'g', 5);
        text += QString("fk(%1) = %2, ").arg(i+1)
                .arg(parameters.laurentians[i].k[LaurFK], 0, 'g', 5);
        text += QString("Gk(%1) = %2\n").arg(i+1)
                .arg(parameters.laurentians[i].k[LaurGK], 0, 'g', 5);
    }
    text += "\n";
    text += QString("N = %1 [cm-3]\nTr = %2 [s]\nRop = %3 [uohm cm]\n")
            .arg(m_paramedit->nbrOfCharge()).arg(m_paramedit->relaxTime()).arg(m_paramedit->opticResistivity());


    painter.drawText(QRect(0, y, w, printer.height() - y), Qt::AlignLeft, text);


    setGraphicStyle();
    functionGestion();
    ui->graphicsViewReal->resizeScene();
    ui->graphicsViewImag->resizeScene();
    ui->graphicsViewRefl->resizeScene();

    QMessageBox::information(this, tr("PDF generate"), tr("PDF file successfully generated"));
}

void MainWindow::on_actionOptimist_dock_triggered(bool checked)
{
    if (m_dockOptimist->isVisible() != checked)
        m_dockOptimist->setVisible(checked);
}

void MainWindow::on_actionCheck_update_triggered()
{
    checkUpdate(true);
}

void MainWindow::on_actionAbout_Ellipsofit_triggered()
{
    AboutEllipsofit().exec();
}

void MainWindow::on_action_Options_triggered()
{
    OptionDialog d;
    d.setPsiDelta(psidelta);
    d.setBlindZoom(m_sceneReal->state() & XYScene::SendZoomChanged);
    for (int st = 0; st < 2; ++st)
        for (int i = 0; i < 7; ++i)
            d.setColor(st, i, m_colors[st][i]);
    if (d.exec() == QDialog::Accepted) {
        QSettings set;

        psidelta = d.psiDelta();
        set.setValue("psidelta_data", psidelta);

        if (d.blindZoom()) {
            m_sceneReal->setState(m_sceneImag->state() | XYScene::SendZoomChanged);
            m_sceneImag->setState(m_sceneImag->state() | XYScene::SendZoomChanged);
        } else {
            m_sceneReal->setState(m_sceneImag->state() &~ XYScene::SendZoomChanged);
            m_sceneImag->setState(m_sceneImag->state() &~ XYScene::SendZoomChanged);
        }
        set.setValue("horizontal_zoom_bind", d.blindZoom());

        for (int st = 0; st < 2; ++st)
            for (int i = 0; i < 7; ++i) {
                m_colors[st][i] = d.color(st, i);
                set.setValue("xycolorstyle_" + QString::number(st) + QString::number(i), m_colors[st][i]);
            }
        setGraphicStyle();
        functionGestion();
    }
}

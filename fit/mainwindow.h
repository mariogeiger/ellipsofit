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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "xyscene.h"
#include "paramedit.h"
#include "optimist.h"

class QHttp;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool loadData(const QString &file);
    bool saveResults(const QString &file);
    bool openResults(const QString &file);

private:
    void synchroCurrentFiles();
    void connexions();
    void setGraphicStyle(int = 0);
    void checkUpdate(bool verbose);

private slots:
    void on_action_Options_triggered();
    void on_actionAbout_Ellipsofit_triggered();
    void on_actionCheck_update_triggered();
    bool httpRequestFinishedSilent(int,bool);
    void httpRequestFinishedVerbose(int,bool);
    void optimizationChangeState(bool);
    void xymouseClic(QPointF rpos, Qt::MouseButtons);
    void functionGestion();
    void xysamexzoom();

    void on_actionOptimist_dock_triggered(bool checked);
    void on_action_Print_triggered();
    void on_actionOpen_results_file_triggered();
    void on_action_Fullscreen_triggered(bool checked);
    void on_action_Save_results_triggered();
    void on_actionLoad_data_triggered();

private:
    Ui::MainWindow *ui;
    XYScene *m_sceneReal;
    XYScene *m_sceneImag;

    Paramedit *m_paramedit;
    QDockWidget *m_dockParamedit;
    Optimist *m_optimist;
    QDockWidget *m_dockOptimist;

    QString m_currentDataFile;
    QString m_currentOpenFile;
    QString m_currentSaveFile;
    QString m_currentPrintFile;
    XYScatterplot *m_dataReal;
    XYScatterplot *m_dataImag;
    XYRealFun *m_funcReal;
    XYRealDrud *m_drudReal;
    XYImagFun *m_funcImag;
    XYImagDrud *m_drudImag;
    QList<XYRealLaurentian *> m_laurentiansReal;
    QList<XYImagLaurentian *> m_laurentiansImag;

    QByteArray m_bufferData;
    int m_httpGetId;
    QHttp *m_http;

    bool psidelta;

    QColor m_colors[2][7];
    int m_st;
};

#endif // MAINWINDOW_H

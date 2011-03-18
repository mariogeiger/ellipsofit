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

#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class QResizeEvent;
class QTimer;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = 0);
    explicit GraphicsView(QGraphicsScene *scene, QWidget *parent = 0);

protected:
    virtual void resizeEvent(QResizeEvent *);

public slots:
    void resizeScene();

private:
	inline void initialisation();
    QTimer *m_timer;
};

#endif // GRAPHICSVIEW_H

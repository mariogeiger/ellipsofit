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

#include "graphicsview.h"
#include <QResizeEvent>
#include <QTimer>

GraphicsView::GraphicsView(QWidget *parent) :
        QGraphicsView(parent)
{
	initialisation();
}

GraphicsView::GraphicsView(QGraphicsScene *scene, QWidget *parent) :
		QGraphicsView(scene, parent)
{
	initialisation();
}

inline void GraphicsView::initialisation()
{
	setFocusPolicy(Qt::WheelFocus);

	m_timer = new QTimer(this);
	m_timer->setInterval(100);
	m_timer->setSingleShot(true);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(resizeScene()));
}

void GraphicsView::resizeEvent(QResizeEvent *e)
{
    if (scene())
        m_timer->start();

    QGraphicsView::resizeEvent(e);
}

void GraphicsView::resizeScene()
{
    if (scene())
        scene()->setSceneRect(geometry());
}


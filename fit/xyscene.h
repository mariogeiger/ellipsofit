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

#ifndef XYSCENE_H
#define XYSCENE_H

#include "realzoom.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsRectItem>

class XYFunction;
class XYScatterplot;
struct XYLook
{
	QPen axesPen;
	QPen subaxesPen;
	QColor textColor;
	QPen zoomPen;
	QBrush backgroundBrush;
};

class XYScene : public QGraphicsScene
{
	Q_OBJECT
public:
	enum Status {
		// Par defaut
		SendMouseMove        = 0x1000, //             Envoie les coordonées souris par le signal 'coordonneesSouris(QPointF)'
		RegraphOnResize      = 0x2000, //     x       Redessine quand il recoit le signal 'sceneRectChanged(QRectF)'
                SendZoomChanged      = 0x4000, //             Envoie un signal
	};

	explicit XYScene(QObject *parent = 0);
	virtual ~XYScene();

	inline QList<XYFunction *> &getFunctionsList();
	inline void appendFunction(XYFunction *); // similaire getFunctionsList().append(...);
	inline QList<XYScatterplot *> &getScatterplotList();
	inline void appendScatterplot(XYScatterplot *); // similaire getScatterplotList().append(...);

	const RealZoom &zoom() const;
	void setZoom(const RealZoom &zoom);
        void setZoom(qreal xmin, qreal xmax, qreal ymin, qreal ymax);

	void relativeZoom(qreal k); // si k < 1.0 ca zoom sinon ca dezoom
	void autoZoom(); // zoom automatique autour des XYScatterplot
	void focusOn(qreal x, qreal y);

	inline int state() const;
	inline void setState(int);

	inline void setAxesPen(const QPen &pen);
	inline void setSubaxesPen(const QPen &pen);
	inline void setTextColor(const QColor &color);
	inline void setZoomPen(const QPen &pen);
	inline void setLook(const struct XYLook &look);
	inline struct XYLook look() const;

signals:
	void mousePosition(QPointF rpos);
	void mouseClic(QPointF rpos, Qt::MouseButtons = 0);
        void zoomChanged();

public slots:
	void regraph();

private slots:
	void maiRegraph();

private:
	void drawaxes();
	void drawfunctions();
	void drawpoints();

	inline qreal xr2i(qreal xr) const;
	inline qreal xi2r(qreal xi) const;
	inline qreal yr2i(qreal yr) const;
	inline qreal yi2r(qreal yi) const;
	inline qreal wr2i(qreal wr) const;
	inline qreal wi2r(qreal wi) const;
	inline qreal hr2i(qreal hr) const;
	inline qreal hi2r(qreal hi) const;
	inline QPointF real2image(const QPointF &p) const;
	inline QPointF image2real(const QPointF &p) const;
	inline QRectF real2image(const RealZoom &r) const;
	inline RealZoom image2real(const QRectF &r) const;

	void keyPressEvent(QKeyEvent *keyEvent);
	void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);

	void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
	QTimer *m_timer;
        QTimer *m_timer2;

	QPointF m_zoomRectOrigin;
	QGraphicsRectItem *m_zoomRect;

	QPen m_axesPen;
	QPen m_subaxesPen;
	QColor m_textColor;
	QPen m_zoomPen;
	//	struct XYLook m_look;

	RealZoom m_realSceneRect;

	QList<XYFunction *> m_functions;
	QList<XYScatterplot *> m_scatterplots;

	int m_state;
	Qt::MouseButtons m_mouseDontMove;
};

class XYFunction
{
public:
	inline XYFunction(const QPen &pen = QPen());
	inline void setPen(const QPen &pen);
	virtual qreal y(qreal x) const = 0;
	virtual bool domain(qreal x) const;

private:
	QPen m_pen;

	friend class XYScene;
};

class XYScatterplot : public QList<QPointF>
{
public:
	inline XYScatterplot(const QPen &pen = QPen(), const QBrush &brush = QBrush(), qreal r = 2.0, const QPen &linePen = QPen(Qt::NoPen));
	inline XYScatterplot(const QList<QPointF> &points, const QPen &pen = QPen(), const QBrush &brush = QBrush(), qreal r = 2.0, const QPen &linePen = QPen(Qt::NoPen));
	inline QList<QPointF> &getPoints();
	inline void setPen(const QPen &pen);
	inline void setBrush(const QBrush &brush);
	inline void setRadius(qreal r);
	inline void setLinePen(const QPen &pen);

private:
	//  QList<QPointF> pts;
	QPen m_pen;
	QBrush m_brush;
	qreal m_r;
	QPen m_linepen;

	friend class XYScene;
};


/* INLINE IMPLANTATION */

XYFunction::XYFunction(const QPen &pen)
	: m_pen(pen)
{
}

void XYFunction::setPen(const QPen &pen)
{
	m_pen = pen;
}


/* INLINE IMPLANTATION */

XYScatterplot::XYScatterplot(const QPen &pen, const QBrush &brush, qreal r, const QPen &linePen)
	: m_pen(pen), m_brush(brush), m_r(r), m_linepen(linePen)
{
}

XYScatterplot::XYScatterplot(const QList<QPointF> &points, const QPen &pen, const QBrush &brush, qreal r, const QPen &linePen)
	: QList<QPointF>(points), m_pen(pen), m_brush(brush), m_r(r), m_linepen(linePen)
{
}

QList<QPointF> &XYScatterplot::getPoints()
{
	return *this;
}

void XYScatterplot::setPen(const QPen &pen)
{
	m_pen = pen;
}

void XYScatterplot::setBrush(const QBrush &brush)
{
	m_brush = brush;
}

void XYScatterplot::setRadius(qreal r) {
	m_r = r;
}

void XYScatterplot::setLinePen(const QPen &pen) {
	m_linepen = pen;
}


/* INLINE IMPLANTATION */

QList<XYFunction *> &XYScene::getFunctionsList() {
	return m_functions;
}

void XYScene::appendFunction(XYFunction *ptr) {
	getFunctionsList().append(ptr);
}

QList<XYScatterplot *> &XYScene::getScatterplotList() {
	return m_scatterplots;
}

void XYScene::appendScatterplot(XYScatterplot *ptr) {
	getScatterplotList().append(ptr);
}

int XYScene::state() const {
	return m_state;
}

void XYScene::setState(int st) {
	m_state = st;
}

void XYScene::setAxesPen(const QPen &pen) {
	m_axesPen = pen;
}

void XYScene::setSubaxesPen(const QPen &pen) {
	m_subaxesPen = pen;
}

void XYScene::setTextColor(const QColor &color) {
	m_textColor = color;
}

void XYScene::setZoomPen(const QPen &pen) {
	m_zoomPen = pen;
}

void XYScene::setLook(const struct XYLook &look) {
	m_axesPen = look.axesPen;
	m_subaxesPen = look.subaxesPen;
	m_textColor = look.textColor;
	m_zoomPen = look.zoomPen;
	setBackgroundBrush(look.backgroundBrush);
}

struct XYLook XYScene::look() const {
	struct XYLook look;
	look.axesPen = m_axesPen;
	look.subaxesPen = m_subaxesPen;
	look.textColor = m_textColor;
	look.zoomPen = m_zoomPen;
	look.backgroundBrush = backgroundBrush();
	return look;
}

qreal XYScene::xr2i(qreal xr) const {
	return (xr - m_realSceneRect.xMin()) / m_realSceneRect.width() * (sceneRect().width() - 1) + sceneRect().left();
}

qreal XYScene::xi2r(qreal xi) const {
	return (xi - sceneRect().left()) / (sceneRect().width() - 1) * m_realSceneRect.width() + m_realSceneRect.xMin();
}

qreal XYScene::yr2i(qreal yr) const {
	return - (yr - m_realSceneRect.yMax()) / m_realSceneRect.height() * (sceneRect().height() - 1) + sceneRect().top();
}

qreal XYScene::yi2r(qreal yi) const {
	return - (yi - sceneRect().top()) / (sceneRect().height() - 1) * m_realSceneRect.height() + m_realSceneRect.yMax();
}

qreal XYScene::wr2i(qreal wr) const {
	return wr / m_realSceneRect.width() * (sceneRect().width() - 1);
}

qreal XYScene::wi2r(qreal wi) const {
	return wi / (sceneRect().width() - 1) * m_realSceneRect.width();
}

qreal XYScene::hr2i(qreal hr) const {
	return hr / m_realSceneRect.height() * (sceneRect().height() - 1);
}

qreal XYScene::hi2r(qreal hi) const {
	return hi / (sceneRect().height() - 1) * m_realSceneRect.height();
}

QPointF XYScene::real2image(const QPointF &real) const {
	return QPointF(xr2i(real.x()), yr2i(real.y()));
}

QPointF XYScene::image2real(const QPointF &image) const {
	return QPointF(xi2r(image.x()), yi2r(image.y()));
}

QRectF XYScene::real2image(const RealZoom &real) const {
	return QRectF(QPointF(xr2i(real.xMin()), yr2i(real.yMax())), QPointF(xr2i(real.xMax()), yr2i(real.yMin())));
}

RealZoom XYScene::image2real(const QRectF &image) const {
	return RealZoom(xi2r(image.left()), xi2r(image.right()), yi2r(image.bottom()), yi2r(image.top()));
}

#endif // XYSCENE_H

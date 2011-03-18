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

#ifndef REALZOOM_H
#define REALZOOM_H

#include <qglobal.h>

class RealZoom
{
public:
	RealZoom();
	RealZoom(qreal xMin, qreal xMax, qreal yMin, qreal yMax);
	inline qreal xMin() const;
	inline qreal xMax() const;
	inline qreal yMin() const;
	inline qreal yMax() const;
	inline qreal width() const;
	inline qreal height() const;

//	inline qreal &rxMin();
//	inline qreal &rxMax();
//	inline qreal &ryMin();
//	inline qreal &ryMax();
	inline void setXMin(qreal);
	inline void setXMax(qreal);
	inline void setYMin(qreal);
	inline void setYMax(qreal);

private:
	qreal _xmin;
	qreal _xmax;
	qreal _ymin;
	qreal _ymax;
	qreal _width;
	qreal _height;
};

inline RealZoom::RealZoom()
	: _xmin(-10.0), _xmax(10.0), _ymin(-10.0), _ymax(10.0)
{
	_width = _xmax - _xmin;
	_height = _ymax - _ymin;
}

inline RealZoom::RealZoom(qreal xMin, qreal xMax, qreal yMin, qreal yMax)
	: _xmin(xMin), _xmax(xMax), _ymin(yMin), _ymax(yMax)
{
	_width = _xmax - _xmin;
	_height = _ymax - _ymin;
}

inline qreal RealZoom::xMin() const {
	return _xmin;
}

inline qreal RealZoom::xMax() const {
	return _xmax;
}

inline qreal RealZoom::yMin() const {
	return _ymin;
}

inline qreal RealZoom::yMax() const {
	return _ymax;
}

inline qreal RealZoom::width() const {
	return _width;
}

inline qreal RealZoom::height() const {
	return _height;
}

//inline qreal &RealZoom::rxMin() {
//	return m_xmin;
//}
//
//inline qreal &RealZoom::rxMax() {
//	return m_xmax;
//}
//
//inline qreal &RealZoom::ryMin() {
//	return m_ymin;
//}
//
//inline qreal &RealZoom::ryMax() {
//	return m_ymax;
//}

inline void RealZoom::setXMin(qreal x) {
	_xmin = x;
	_width = _xmax - _xmin;
}

inline void RealZoom::setXMax(qreal x) {
	_xmax = x;
	_width = _xmax - _xmin;
}

inline void RealZoom::setYMin(qreal y) {
	_ymin = y;
	_height = _ymax - _ymin;
}

inline void RealZoom::setYMax(qreal y) {
	_ymax = y;
	_height = _ymax - _ymin;
}

#endif // REALZOOM_H

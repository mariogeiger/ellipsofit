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

#ifndef ELLIPSOMATH_H
#define ELLIPSOMATH_H

#include "xyscene.h"

typedef struct {
	double k[3];
} Laurentian;

enum {
	LaurEK = 0,
	LaurFK = 1,
	LaurGK = 2,
};

typedef struct {
    double einf;
    double ep;
    double g;
	QList<Laurentian> laurentians;
} Parameters;

extern Parameters parameters;



//
// main Class
//

class XYEllipsoFun : public XYFunction
{
public:
	XYEllipsoFun(const Parameters &parameters, const QPen &pen = QPen());
	virtual bool domain(qreal x) const;

protected:
	const Parameters &p;
};



//
// Real classes
//

class XYRealFun : public XYEllipsoFun
{
public:
	XYRealFun(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;

};
class XYRealDrud : public XYEllipsoFun
{
public:
	XYRealDrud(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;

};
class XYRealLaurentian : public XYEllipsoFun
{
public:
	XYRealLaurentian(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;
	void setNo(int n);
private:
	int no;
};

qreal mathRealFun(qreal e, const Parameters &p);



//
// Imaginary classes
//

class XYImagFun : public XYEllipsoFun
{
public:
	XYImagFun(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;
};

class XYImagDrud : public XYEllipsoFun
{
public:
	XYImagDrud(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;
};

class XYImagLaurentian : public XYEllipsoFun
{
public:
	XYImagLaurentian(const Parameters &parameters, const QPen &pen = QPen());
	virtual qreal y(qreal x) const;
	void setNo(int n);
private:
	int no;
};

qreal mathImagFun(qreal e, const Parameters &p);



//
// Reflectivity classes
//

class XYReflFun : public XYEllipsoFun
{
public:
        XYReflFun(const Parameters &parameters, const QPen &pen = QPen());
        virtual qreal y(qreal x) const;
};

qreal mathReflectivity(qreal e, const Parameters &p);




#endif // ELLIPSOMATH_H

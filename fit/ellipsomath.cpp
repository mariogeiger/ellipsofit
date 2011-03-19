/*
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "ellipsomath.h"
#include <math.h>



//==============================================================
// MATHEMATIC - REAL
//==============================================================

qreal mathRealDrud(qreal e, qreal einf, qreal ep, qreal g) {
    return einf - (ep * ep) / (e * e + g * g);
}

qreal mathRealLaurentian(qreal e, qreal ek, qreal fk, qreal gk) {
    const qreal quadE = e * e;
    const qreal quadEk = ek * ek;
    const qreal delta = quadEk - quadE;

    return (fk * quadEk * delta) / (delta * delta + gk * gk * quadE);
}

qreal mathRealFun(qreal e, const Parameters &p)
{
    qreal som = mathRealDrud(e, p.einf, p.ep, p.g);
    for (int i = 0; i < p.laurentians.size(); ++i)
        som += mathRealLaurentian(e, p.laurentians[i].k[LaurEK],
                                  p.laurentians[i].k[LaurFK],
                                  p.laurentians[i].k[LaurGK]);
    return som;
}



//==============================================================
// MATHEMATIC - IMAGINARY
//==============================================================

qreal mathImagDrud(qreal e, qreal ep, qreal g) {
    const qreal quadE = e * e;
    return (ep * ep * g * e) / (quadE * quadE + g * g * quadE);
}

qreal mathImagLaurentian(qreal e, qreal ek, qreal fk, qreal gk) {
    const qreal quadE = e * e;
    const qreal quadEk = ek * ek;
    const qreal delta = quadEk - quadE;

    return (fk * quadEk * gk * e) / (gk * gk * quadE + delta * delta);
}

qreal mathImagFun(qreal e, const Parameters &p)
{
    qreal som = mathImagDrud(e, p.ep, p.g);
    for (int i = 0; i < p.laurentians.size(); ++i)
        som += mathImagLaurentian(e, p.laurentians[i].k[LaurEK],
                                  p.laurentians[i].k[LaurFK],
                                  p.laurentians[i].k[LaurGK]);
    return som;
}



//==============================================================
// MATHEMATIC - REFLEXION
//==============================================================

qreal mathReflexion(qreal e, const Parameters &p)
{
    const qreal epsilon1 = mathRealFun(e, p);
    const qreal epsilon2 = mathImagFun(e, p);
    //! FIXME
    return epsilon1 / epsilon2;
}



//==============================================================
// GLOBALES & MACRO
//==============================================================

Parameters parameters;

XYEllipsoFun::XYEllipsoFun(const Parameters &parameters, const QPen &pen)
    : XYFunction(pen), p(parameters)
{

}

bool XYEllipsoFun::domain(qreal x) const
{
    if (x < 0.1)
        return false;
    else
        return true;
}




//==============================================================
// GRAPHIC - REAL
//==============================================================

XYRealFun::XYRealFun(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen)
{

}

qreal XYRealFun::y(qreal x) const
{
    return mathRealFun(x, p);
}

XYRealDrud::XYRealDrud(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen)
{

}

qreal XYRealDrud::y(qreal x) const
{
    return mathRealDrud(x, p.einf, p.ep, p.g);
}

XYRealLaurentian::XYRealLaurentian(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen), no(0)
{

}

qreal XYRealLaurentian::y(qreal x) const
{
    return mathRealLaurentian(x, p.laurentians[no].k[LaurEK],
                              p.laurentians[no].k[LaurFK],
                              p.laurentians[no].k[LaurGK]);
}

void XYRealLaurentian::setNo(int n)
{
    no = n;
}


//==============================================================
// GRAPHIC - IMAGINARY
//==============================================================

XYImagFun::XYImagFun(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen)
{

}

qreal XYImagFun::y(qreal x) const
{
    return mathImagFun(x, p);
}

XYImagDrud::XYImagDrud(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen)
{

}

qreal XYImagDrud::y(qreal x) const
{
    return mathImagDrud(x, p.ep, p.g);
}

XYImagLaurentian::XYImagLaurentian(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen), no(0)
{

}

qreal XYImagLaurentian::y(qreal x) const
{
    return mathImagLaurentian(x, p.laurentians[no].k[LaurEK],
                              p.laurentians[no].k[LaurFK],
                              p.laurentians[no].k[LaurGK]);
}

void XYImagLaurentian::setNo(int n)
{
    no = n;
}


//==============================================================
// GRAPHIC - REFLEXION
//==============================================================

XYReflFun::XYReflFun(const Parameters &parameters, const QPen &pen)
    : XYEllipsoFun(parameters, pen)
{

}

qreal XYReflFun::y(qreal x) const
{
    return mathReflexion(x, p);
}

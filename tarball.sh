#! /bin/sh

name='ellipsofit'
version=$1
tarname=${name}_${version}.orig.tar.gz
dirname=${name}-${version}

cd "../EllipsoFit-packaging"

mkdir $dirname || exit 0

qtar -v -i "../EllipsoFit/EllipsoFit.pro" -o "tmp.tar" || exit 0

tar xf tmp.tar

cp -r EllipsoFit/* $dirname

tar zcf $tarname $dirname

rm -r EllipsoFit
rm tmp.tar


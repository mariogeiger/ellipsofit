#! /bin/sh

echo -n "version string : "
read version

cd ellipsofit-$version || exit 0


gpg --list-keys
echo -n "gpg key : "
read gpgkey

debuild -k0x$gpgkey -S -sa --lintian-opts -i || exit 0
cd ..


for arch in $*
do

	clear
	echo "=== BEGIN $arch ==="
	sudo pbuilder --clean || exit 0
	sudo ARCH=$arch pbuilder --create --architecture $arch || exit 0
	sudo ARCH=$arch pbuilder --build --architecture $arch --buildresult . ellipsofit_${version}-1.dsc || exit 0

done


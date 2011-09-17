#! /bin/sh

echo -n "version string : "
read version

#cd ellipsofit-$version || exit 0


gpg --list-keys
echo -n "gpg key : "
read gpgkey




# -k<gpgkey>                       construire un paquet pour un envoi sponsorisé
# -S                                ?
# -sa                               ?
# --lintian-opts                    lintian option used to run lintian
# lintian -i, --info                give detailed info about tags

debuild -k0x$gpgkey -S -sa --lintian-opts -i || exit 0



cd ..


for arch in $*
do

	clear
	echo "=== BEGIN $arch pbuilder --clean ==="
	
	sudo ARCH=$arch pbuilder --clean || exit 0

	clear
	echo "=== BEGIN $arch pbuilder update ==="
	
	sudo ARCH=$arch pbuilder update --debbuildopts "-I -i -j5 " || exit 0

	clear
	echo "=== BEGIN $arch pbuilder --create ==="
	
	sudo ARCH=$arch DEB_BUILD_OPTIONS="parallel=5" pbuilder --create --architecture $arch --debbuildopts "-I -i -j5 " || exit 0

	clear
	echo "=== BEGIN $arch pbuilder --build ==="
	
	sudo ARCH=$arch DEB_BUILD_OPTIONS="parallel=5" pbuilder --build --architecture $arch --debbuildopts "-I -i -j5 " --buildresult . ellipsofit_${version}-1.dsc || exit 0

done


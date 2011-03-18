#! /bin/bash

srcfile='/etc/apt/sources.list'
debline=''

function append
{
	if ! grep "$debline" "$srcfile" > /dev/null
	then
		echo $debline >> /etc/apt/sources.list
	else
		echo "$debline already in $srcfile"
	fi
}





debline='deb http://setup.weeb.ch/debian unstable main'
append

debline='deb-src http://setup.weeb.ch/debian unstable main'
append


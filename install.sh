#!/bin/bash
# Check and install library dependencies

# Check for SDL library
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libsdl2-dev | grep "install ok installed")
if [ "" == "$PKG_OK" ];
then
	gksudo apt-get install --yes --force-yes libsdl2-dev
fi

# Check for JPEG library
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' libjpeg-dev | grep "install ok installed")
if [ "" == "$PKG_OK" ];
then
	gksudo apt-get install --yes --force-yes libjpeg-dev
fi

#!/bin/bash

#Use of script ./build_kernel.sh modelno variant ex. ./build_kernel.sh 910 G (for N910G)

#Remove old output directory
#rm -r $(pwd)/output

#Define defconfig for the selected model

if [ "$1" = "910" ]; then 
	model=trlte
fi
if [ "$1" = "915" ]; then 
	model=tblte
fi

#Define defconfig for the selected variant

if [[ ( "$2" = "G" ) || ( "$2" = "F" ) || ( "$2" = "FY" ) ]]; then 
	variant=eur
fi
if [ "$2" = "P" ]; then 
	variant=spr
fi
if [ "$2" = "T" ]; then 
	variant=tmo
fi
if [ "$2" = "V" ]; then 
	variant=vzw
fi
if [ "$2" = "W8" ]; then 
	variant=can
fi

#Main build_kernel.sh script

export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/gcc-linaro-6.1.1-2016.08-x86_64_arm-eabi/bin/arm-eabi-
mkdir output

make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_"$model"_"$variant"_defconfig apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output

cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage

if [ "$3" = "N" ]; then
    echo "Not making zip"
    exit
else
./RamScript.sh $1$2 #Run script for making zip
fi

#Rerun script for zip in case of same zimage is used for both models

if [ "$2" = "G" ]; then
	./RamScript.sh $1F
fi
if [[ ( "$1" = "910" ) && ( "$2" = "F" ) ]]; then
	./RamScript.sh $1G
fi
if [[ ( "$1" = "915" ) && ( "$2" = "F" ) ]]; then
	./RamScript.sh $1$2Y
fi
if [[ ( "$1" = "915" ) && ( "$2" = "FY" ) ]]; then
	./RamScript.sh $1F
fi

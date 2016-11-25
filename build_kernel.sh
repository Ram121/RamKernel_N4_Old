#!/bin/bash

#Use of script ./build_kernel.sh modelno ex. ./build_kernel.sh G (for N910G)

#Remove old output directory
rm -r $(pwd)/output

#Define defconfig for the selected model

if [ "$1" = "G" ]; then 
	model=eur_defconfig
fi
if [ "$1" = "F" ]; then 
	model=eur_defconfig
fi
if [ "$1" = "P" ]; then 
	model=spr_defconfig
fi
if [ "$1" = "T" ]; then 
	model=tmo_defconfig
fi
if [ "$1" = "V" ]; then 
	model=vzw_defconfig
fi
if [ "$1" = "W8" ]; then 
	model=can_defconfig
fi

#Rename Models Makefile to Makefile for compiling

mv Makefile_N910$1 Makefile

#Main build_kernel.sh script

export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/gcc-linaro-6.1.1-2016.08-x86_64_arm-eabi/bin/arm-eabi-
mkdir output

make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_trlte_$model apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output

cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage

#Rename back the Makefile to model number

mv Makefile Makefile_N910$1

#Run script for making zip

./RamScript.sh $1

#Rerun script for zip in case of Model F or G as same zimage is used for both models

if [ "$1" = "G" ]; then
	./RamScript.sh F
fi
if [ "$1" = "F" ]; then
	./RamScript.sh G
fi

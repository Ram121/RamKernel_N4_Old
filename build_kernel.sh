#!/bin/bash

#Use of script ./build_kernel.sh modelno ex. ./build_kernel.sh G (for N910G)

if [ "$1" = "G" ]; then 
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

mv Makefile_N910$1 Makefile
export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/gcc-linaro-5.1_arm-eabi/bin/arm-eabi-
mkdir output

make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_trlte_$model apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output

cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage

mv Makefile Makefile_N910$1

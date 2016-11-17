#!/bin/bash

mv Makefile_N910G Makefile
export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/gcc-linaro-5.1_arm-eabi/bin/arm-eabi-
mkdir output

make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_trlte_eur_defconfig apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output

cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage

mv Makefile Makefile_N910G

#!/bin/bash

mv Makefile_N910G Makefile
export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/UberTC-arm-eabi-7.0/bin/arm-eabi-ar
mkdir output

make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_trlte_eur_defconfig apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output

cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage

mv Makefile Makefile_N910G

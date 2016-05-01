#!/bin/bash

#Use of script ./build_kernel.sh modelno variant zip.making selinux   #ex. $./build_kernel.sh N910 G Y 0 #(for N910G Permissive)

#Remove old output directory
rm -r $(pwd)/output
rm arch/arm/boot/zImage
rm bootimg/boot.img-dtb

#Define defconfig for the selected model
if [ "$1" = "N910" ]; then 
	model=trlte
fi
if [ "$1" = "N915" ]; then 
	model=tblte
fi
if [ "$1" = "G901" ]; then 
	model=kccat6
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
if [ "$2" = "D" ]; then 
	variant=dcm
fi
if [ "$2" = "0" ]; then 
	variant=chnzn
fi

#Set selinux
if [ "$4" = "0" ]; then 
	sed -i '187s/new_value = 1;/new_value = 0;/' $(pwd)/security/selinux/selinuxfs.c
	echo ""
	echo "Making permissive kernel for $1$2"
	echo ""
fi
if [ "$4" = "1" ]; then 
	sed -i '187s/new_value = 0;/new_value = 1;/' $(pwd)/security/selinux/selinuxfs.c
	echo ""
	echo "Making enforcing kernel for $1$2"
	echo ""
fi

#Set kernel version
sed -i '8s/CONFIG_LOCALVERSION="-RamKernel_v7"/CONFIG_LOCALVERSION="-RamKernel_b8.2"/' $(pwd)/arch/arm/configs/apq8084_sec_"$model"_"$variant"_defconfig

#Main build_kernel.sh script
export ARCH=arm
export CROSS_COMPILE=/opt/toolchains/UBERTC-arm-eabi-4.8/bin/arm-eabi-
#/opt/toolchains/gcc-linaro-5.1-x86_64_arm-eabi/bin
mkdir output
make -C $(pwd) O=output VARIANT_DEFCONFIG=apq8084_sec_"$model"_"$variant"_defconfig apq8084_sec_defconfig SELINUX_DEFCONFIG=selinux_defconfig
make -C $(pwd) O=output
cp output/arch/arm/boot/Image $(pwd)/arch/arm/boot/zImage
./tools/dtbTool -o ./bootimg/boot.img-dtb -s 4096 -p ./output/scripts/dtc/ ./output/arch/arm/boot/dts/

#Option for making zip or not
if [ "$3" = "N" ]; then
    echo "Not making zip"
    exit
else
./RamScript.sh $1 $2 $4 #Run script for making zip
fi

#Rerun script for zip in case of same zimage is used for both models
if [ "$2" = "G" ]; then
	./RamScript.sh $1 F $4
fi
if [[ ( "$1" = "910" ) && ( "$2" = "F" ) ]]; then
	./RamScript.sh $1 G $4
fi
if [[ ( "$1" = "915" ) && ( "$2" = "F" ) ]]; then
	./RamScript.sh $1 $2Y $4 
fi
if [[ ( "$1" = "915" ) && ( "$2" = "FY" ) ]]; then
	./RamScript.sh $1 F $4
fi

#!/bin/bash

#Script for packing ramdisk and making zip
#Run script in format ./RamScript.sh model variant selinux.status ex. ./RamScript.sh N910 F 0  #for N910F permissive

#Read kernel state (Permissive/Enforcing)
if [ "$3" = "0" ]; then
	selinux=Permissive
fi
if [ "$3" = "1" ]; then
	selinux=Enforcing
fi

#Clear working directory
for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait
rm $(pwd)/bootimg/AIK-Linux-2.4/boot.img

#Copy stock boot img to working directory
cp bootimg/stock_bootimg/$1$2/boot.img $(pwd)/bootimg/AIK-Linux-2.4

#Unpack img
for i in $(pwd)/bootimg/AIK-Linux-2.4/unpackimg.sh
do
"$i" &
done
wait

#Use new kernel in boot img
rm $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage
cp $(pwd)/output/arch/arm/boot/zImage $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

#Patching ramdisk
patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/default.prop $(pwd)/bootimg/patches/defualt.prop/$1.default.prop.patch
rm $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc
rm $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-dtb
cp $(pwd)/bootimg/patches/environ-rc/N7 $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc
cp $(pwd)/bootimg/boot.img-dtb $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-dtb
patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.rc $(pwd)/bootimg/patches/init.d/init.rc.patch
cp $(pwd)/bootimg/patches/init.d/init.d_support.sh $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/

#For synapse support
#cp -r $(pwd)/bootimg/patches/Synapse_support/ramdisk/* $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/
#patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/ueventd.rc $(pwd)/bootimg/patches/Synapse_support/ueventd.rc.patch
#cp $(pwd)/bootimg/patches/Synapse_support/ramdisk_fix_permissions.sh $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/ramdisk_fix_permissions.sh
#cd $(pwd)/bootimg/AIK-Linux-2.4/ramdisk
#chmod 0777 ramdisk_fix_permissions.sh
#./ramdisk_fix_permissions.sh 2>/dev/null
#rm -f ramdisk_fix_permissions.sh
#cd -

#Pack boot img
for i in $(pwd)/bootimg/AIK-Linux-2.4/repackimg.sh
do
"$i" &
done
wait
cp $(pwd)/bootimg/AIK-Linux-2.4/image-new.img $(pwd)/bootimg/zips/template/boot.img

#Cleanup working directory after work
for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait
rm $(pwd)/bootimg/AIK-Linux-2.4/boot.img

#Making zip
7z a -tzip -mx5 $(pwd)/bootimg/zips/RamKernel_$1$2_V7_$selinux.zip $(pwd)/bootimg/zips/template/META-INF $(pwd)/bootimg/zips/template/system $(pwd)/bootimg/zips/template/boot.img #removed $(pwd)/bootimg/zips/template/ram
rm $(pwd)/bootimg/zips/template/boot.img
echo "$selinux zip made for $1$2"

#!/bin/bash

#Script for packing ramdisk and making zip
#Run script in format ./RamScript.sh model variant ex. ./RamScript.sh N910 F  #for N910F

for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait

cp bootimg/stock_bootimg/$1$2/boot.img $(pwd)/bootimg/AIK-Linux-2.4

for i in $(pwd)/bootimg/AIK-Linux-2.4/unpackimg.sh
do
"$i" &
done
wait

rm $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

cp $(pwd)/output/arch/arm/boot/zImage $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/default.prop $(pwd)/bootimg/patches/defualt.prop/$1.default.prop.patch

rm $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc

cp $(pwd)/bootimg/patches/environ-rc/N7 $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc

patch $(pwd)/bootimg/AIK-Linux-2.4/init.d/init.rc $(pwd)/bootimg/patches/Synapse_support/init.rc.patch

cp $(pwd)/bootimg/AIK-Linux-2.4/init.d/init.d_support.sh $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/

#For synapse support
#cp -r $(pwd)/bootimg/patches/Synapse_support/ramdisk/* $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/
#patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/ueventd.rc $(pwd)/bootimg/patches/Synapse_support/ueventd.rc.patch
#cp $(pwd)/bootimg/patches/Synapse_support/ramdisk_fix_permissions.sh $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/ramdisk_fix_permissions.sh
#cd $(pwd)/bootimg/AIK-Linux-2.4/ramdisk
#chmod 0777 ramdisk_fix_permissions.sh
#./ramdisk_fix_permissions.sh 2>/dev/null
#rm -f ramdisk_fix_permissions.sh
#cd -

for i in $(pwd)/bootimg/AIK-Linux-2.4/repackimg.sh
do
"$i" &
done
wait

cp $(pwd)/bootimg/AIK-Linux-2.4/image-new.img $(pwd)/bootimg/zips/template/boot.img

for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait

rm $(pwd)/bootimg/AIK-Linux-2.4/boot.img

7z a -tzip -mx5 $(pwd)/bootimg/zips/RamKernel_$1$2_V6.zip $(pwd)/bootimg/zips/template/META-INF $(pwd)/bootimg/zips/template/system $(pwd)/bootimg/zips/template/boot.img #removed $(pwd)/bootimg/zips/template/ram

rm $(pwd)/bootimg/zips/template/boot.img

echo "Zip made for $1$2"

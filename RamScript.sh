#!/bin/bash

#Added for packing ramdisk and making zip

for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait

cp bootimg/stock_bootimg/N910G/boot.img $(pwd)/bootimg/AIK-Linux-2.4

for i in $(pwd)/bootimg/AIK-Linux-2.4/unpackimg.sh
do
"$i" &
done
wait

rm $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

cp $(pwd)/output/arch/arm/boot/zImage $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

rm -r $(pwd)/output

patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/default.prop $(pwd)/bootimg/patches/default.prop.patch

rm $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc

cp $(pwd)/bootimg/patches/N7 $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc 

for i in $(pwd)/bootimg/AIK-Linux-2.4/repackimg.sh
do
"$i" &
done
wait

cp $(pwd)/bootimg/AIK-Linux-2.4/image-new.img $(pwd)/bootimg/zips/template/boot.img

7z a -tzip -mx5 $(pwd)/bootimg/zips/RamKernel_N7Port_N910G.zip $(pwd)/bootimg/zips/template/META-INF $(pwd)/bootimg/zips/template/root $(pwd)/bootimg/zips/template/system $(pwd)/bootimg/zips/template/boot.img $(pwd)/bootimg/zips/template/data

rm $(pwd)/bootimg/zips/template/boot.img
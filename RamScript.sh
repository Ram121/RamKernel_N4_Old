#!/bin/bash

#Script for packing ramdisk and making zip
#Run script in format ./RamScript.sh model_number ex. ./RamScript.sh F for N910F

for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait

cp bootimg/stock_bootimg/N910$1/boot.img $(pwd)/bootimg/AIK-Linux-2.4

for i in $(pwd)/bootimg/AIK-Linux-2.4/unpackimg.sh
do
"$i" &
done
wait

rm $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

cp $(pwd)/output/arch/arm/boot/zImage $(pwd)/bootimg/AIK-Linux-2.4/split_img/boot.img-zImage

patch $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/default.prop $(pwd)/bootimg/patches/default.prop.patch

rm $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc

cp $(pwd)/bootimg/patches/N7 $(pwd)/bootimg/AIK-Linux-2.4/ramdisk/init.environ.rc 

for i in $(pwd)/bootimg/AIK-Linux-2.4/repackimg.sh
do
"$i" &
done
wait

cp $(pwd)/bootimg/AIK-Linux-2.4/image-new.img $(pwd)/bootimg/zips/template/boot.img

file="$(pwd)/bootimg/zips/template/boot.img"
if [ -f "$file" ]
then
	echo "$file found removing output directory"
	rm -r $(pwd)/output 
else
	echo "$file not found"
fi

for i in $(pwd)/bootimg/AIK-Linux-2.4/cleanup.sh
do
"$i" &
done
wait

rm $(pwd)/bootimg/AIK-Linux-2.4/boot.img

7z a -tzip -mx5 $(pwd)/bootimg/zips/RamKernel_N910$1_All.zip $(pwd)/bootimg/zips/template/META-INF $(pwd)/bootimg/zips/template/system $(pwd)/bootimg/zips/template/boot.img

#removed $(pwd)/bootimg/zips/template/ram

rm $(pwd)/bootimg/zips/template/boot.img

echo "Zip made for N910$1"

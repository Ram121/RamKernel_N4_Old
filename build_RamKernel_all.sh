#!/bin/bash

for i in build_kernel_N910G.sh
do
"$i" &
done
wait

for i in build_kernel_N910P.sh
do
"$i" &
done
wait

for i in build_kernel_N910T.sh
do
"$i" &
done
wait

for i in build_kernel_N910V.sh
do
"$i" &
done
wait

build_kernel_N910W8.sh

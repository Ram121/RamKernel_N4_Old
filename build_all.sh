#!/bin/bash

./build_kernel.sh N910 G Y 0

./build_kernel.sh N910 V Y 0

./build_kernel.sh N910 P Y 0

./build_kernel.sh N910 T Y 0

./build_kernel.sh N910 W8 Y 0

./build_kernel.sh N915 F Y 0

./build_kernel.sh G901 F Y 0

echo "Permissive zip made for all models"

./build_kernel.sh N910 G Y 1

./build_kernel.sh N910 V Y 1

./build_kernel.sh N910 P Y 1

./build_kernel.sh N910 T Y 1

./build_kernel.sh N910 W8 Y 1

./build_kernel.sh N915 F Y 1

./build_kernel.sh G901 F Y 1

echo "Enforcing zip made for all models"

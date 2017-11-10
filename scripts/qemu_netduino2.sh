#!/bin/sh

cpu=arm-cortex-m3

qemu-system-arm -s -M netduino2 --cpu $cpu -kernel cbaos.elf  -monitor none -serial none -semihosting -semihosting-config target=native -nographic

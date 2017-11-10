# this is a configuration file included by SConstruct

NAME = 'cbaos.elf'
ARCH = 'arm-cortex-m0'
CROSSCOMPILE = 'arm-none-eabi'
MACH = 'lpc11xx'
CHIP = 'qemu_cm0'
BOARD = 'qemu_netduino2_cm0'
CONFIG_FCPU = 1000000 # default systick clock in qemu
APPLICATION = 'blinky'

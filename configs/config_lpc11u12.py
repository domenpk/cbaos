# this is a configuration file included by SConstruct

NAME = 'cbaos.elf'
ARCH = 'arm-cortex-m0'
CROSSCOMPILE = 'arm-none-eabi'
MACH = 'lpc11xx'
CHIP = 'lpc11u12'
BOARD = 'lpcxpresso1114'
CONFIG_FCPU = 48000000
#CONFIG_USBDONGLE = 1
#CONFIG_USBDONGLE_PCB1 = 1
#CONFIG_SPI_BRIDGE = 1
#APPLICATION = 'stargate_switch'
APPLICATION = 'blinky'

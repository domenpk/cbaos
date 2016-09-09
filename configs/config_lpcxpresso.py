# this is a configuration file included by SConstruct

NAME = 'cbaos.elf'
ARCH = 'arm-cortex-m3'
CROSSCOMPILE = 'arm-none-eabi'
MACH = 'lpc13xx'
CHIP = 'lpc1343'
BOARD = 'lpcxpresso'
CONFIG_FCPU = 72000000
#CONFIG_USBDONGLE = 1
#CONFIG_USBDONGLE_PCB1 = 1
#CONFIG_SPI_BRIDGE = 1
APPLICATION = 'blinky'
#APPLICATION = 'cbashell'

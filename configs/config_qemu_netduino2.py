# this is a configuration file included by SConstruct

NAME = 'cbaos.elf'
ARCH = 'arm-cortex-m3'
CROSSCOMPILE = 'arm-none-eabi'
MACH = 'stm32'
CHIP = 'stm32f205xf'
BOARD = 'qemu_netduino2'
CONFIG_FCPU = 1000000 # default systick clock in qemu
#LIBC = 'newlib' # with nano.specs it still adds 6k flash

APPLICATION = 'blinky'

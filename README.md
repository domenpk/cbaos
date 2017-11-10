# cbaos
Simple microcontroller OS, with priority-based preemptive scheduling. Currently ported to a few ARM Cortex-Mx chips.

This is not a serious project, so if you use it be prepared to fix whatever is broken.
It is "from scratch", its own CRT, linker scripts, libc - code has no external dependencies. Might be useful for learning about microcontroller bring-up.

## Requirements
You'll need:
- `scons`
- `arm-none-eabi-` toolchain

## Usage
```
./scripts/use_config.py configs/config_qemu_netduino2.py 
scons
./scripts/qemu_netduino2.sh
```

Or use a real target and flash cbaos.{elf,hex,bin} to your microcontroller.

## Design
Some ideas stolen from Linux kernel:
- Tickless scheduler
- Coding style
- Device driver model

## What works
- Last used target architecture should work
- QEMU for netduino2 target

## TODO
- Automated testing for all targets (at least compile)
- Add support for newlib
- 64-bit systick (no wraparound worries)
- Timer support

## Contributing
Pull requests gladly accepted, although there might be some delay.

Do note the project is licensed WTFPL (public domain equivalent).

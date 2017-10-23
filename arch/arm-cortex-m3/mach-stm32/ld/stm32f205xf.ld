MEMORY
{
	/* qemu needs this at 0x0 */
	FLASH (rx) : ORIGIN = 0x0,        LENGTH = 768K
	RAM (rw) :   ORIGIN = 0x20000000, LENGTH = 112K
	SRAM2 (rw) : ORIGIN = 0x2001c000, LENGTH = 16K
}

INCLUDE arch/arm-cortex-m3/ld/sections.ld

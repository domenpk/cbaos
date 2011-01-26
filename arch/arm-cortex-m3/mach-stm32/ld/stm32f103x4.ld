MEMORY
{
	FLASH (rx) :	ORIGIN = 0x08000000,	LENGTH = 16K
	RAM (rw) :	ORIGIN = 0x20000000,	LENGTH = 6K
}

INCLUDE arch/arm-cortex-m3/ld/sections.ld

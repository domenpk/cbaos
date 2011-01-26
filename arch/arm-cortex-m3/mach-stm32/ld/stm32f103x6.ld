MEMORY
{
	FLASH (rx) :	ORIGIN = 0x08000000,	LENGTH = 32K
	RAM (rw) :	ORIGIN = 0x20000000,	LENGTH = 10K
}

INCLUDE arch/arm-cortex-m3/ld/sections.ld

MEMORY
{
	FLASH (rx) :	ORIGIN = 0,		LENGTH = 32K
	RAM (rw) :	ORIGIN = 0x20000000,	LENGTH = 8K
}

INCLUDE arch/arm-cortex-m3/ld/sections.ld

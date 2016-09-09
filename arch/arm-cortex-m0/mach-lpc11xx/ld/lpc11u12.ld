MEMORY
{
	FLASH (rx) :	ORIGIN = 0,		LENGTH = 16K
	RAM (rw) :	ORIGIN = 0x10000000,	LENGTH = 4K
}

INCLUDE arch/arm-cortex-m3/ld/sections.ld

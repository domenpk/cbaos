MEMORY
{
	FLASH (rx) :	ORIGIN = 0,		LENGTH = 512K
	RAM (rw) :	ORIGIN = 0x40000000,	LENGTH = 32K
}

INCLUDE arch/arm7tdmi/ld/sections.ld

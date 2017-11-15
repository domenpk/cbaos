ENTRY(_vectors)

/* http://sourceware.org/binutils/docs/ld/Output-Section-LMA.html */

_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
	.text :
	{
		KEEP(*(.vectors))

		/* because this is only accessible after remapping from boot loader */
		/* TODO hmm? still true? can .rodata be before this? where is it remapped? */
		. = . < 0x200 ? 0x200 : . ;

		*(.text)
		*(.rodata)
		*(.rodata.*)
	} >FLASH

	/* does this guarantee nothing will come between? */
	. = ALIGN(4);
	_fldata = . ;
	.data : ALIGN(4)
	{
		_data = . ;
		*(.data)
		. = ALIGN(4);
		_edata = . ;
	} >RAM AT>FLASH

	.bss : ALIGN(4)
	{
		_bss = . ;
		*(.bss)
		*(COMMON)
		. = ALIGN(4);
		_ebss = ALIGN(4);
	} >RAM

	.heap(COPY):
	{
		PROVIDE(end = .); /* for newlib */
		*(.heap*)
	} > RAM
/*	blah :
	{
		*(.init*)
		*(.fini*)
		*(.eh_frame)
		*(.jcr)
		*(.bss)
	} >RAM*/

	_end = ALIGN(4);
}

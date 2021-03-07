ENTRY(entry_point)

SECTIONS
{	
	. = 0x100000;
	
	kernel_start = .;
		
	.text : {
		*(.text .text.*)
	}
	
	.rodata : {
		*(.rodata .rodata.*)
	}
	
	.data : {
		*(.data .data.*)
	}
	
	.bss : {
		. = ALIGN(16);
		*(.bss .bss.*)
		*(COMMON)
	}
	
	kernel_end = .;
}

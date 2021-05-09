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
		start_ctors = .;
		KEEP(*( .init_array ));
		KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
		end_ctors = .;

		*(.data)
	}
	
	.bss : {
		. = ALIGN(16);
		*(.bss .bss.*)
		*(COMMON)
	}
	
	kernel_end = .;
}

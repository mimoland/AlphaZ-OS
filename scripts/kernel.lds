ENTRY(_start)
SECTIONS
{

	. = 0xc0000000 + 0x100000;
	.text :
	{
		_text = .;
        *(.head.text);
		*(.text)
		_etext = .;
	}

	. = ALIGN(8);

	.data :
	{
		_data = .;
		*(.data)
		_edata = .;
	}

	.rodata :
	{
		_rodata = .;
		*(.rodata)
		_erodata = .;
	}

	.bss :
	{
		_bss = .;
		*(.bss)
		_ebss = .;
	}

	_end = .;
}

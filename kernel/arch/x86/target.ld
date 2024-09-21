/* target.ld */
ENTRY(_start)

_mb_phys_base = 0x00400000;   /* phys 4MB aligned. */
_mb_virt_base = 0xF0400000;   /* virtual 4MB. */
_mb_addr_offset = _mb_virt_base - _mb_phys_base;

SECTIONS
{
	. = _mb_phys_base;
    .multiboot : {
		*(.unpaged.multiboot)
    }

    /* unpaged area */
    _mb_unpaged_start = .;
    .unpaged.text ALIGN(4K) : {
        *(.unpaged.text)
    }
    
    .unpaged.data ALIGN(4K) : {
        *(.unpaged.data)
    }
    
    .unpaged.bss ALIGN(4K) : {
		*(.unpaged.COMMON)
		*(.unpaged.kstack)
        *(.unpaged.bss)
    }

    _mb_unpaged_end = .;

    /* paged area */
    . += _mb_addr_offset;
    _mb_start = .;

	.text ALIGN(4K) : AT(ADDR(.text) - _mb_addr_offset)
	{
		*(.text)
	}

	.rodata ALIGN(4K) : AT(ADDR(.rodata) - _mb_addr_offset)
	{
		*(.rodata)
	}

	.data ALIGN(4K) : AT(ADDR(.data) - _mb_addr_offset)
	{
		*(.data)
	}

	.bss ALIGN(4K) : AT(ADDR(.bss) - _mb_addr_offset)
	{
		_kinit_head = .;
		*(SORT(.ctors.*))
		*(SORT(.ctors))
		*(SORT(.ctor*))
		_kinit_tail = .;

		*(COMMON)
		*(.kstack)
		*(.bss)

		_mb_size = . - _mb_virt_base;
	}
	
    _mb_end = .;
}
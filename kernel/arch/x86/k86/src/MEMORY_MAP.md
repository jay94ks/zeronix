## Memory Map.

1. grub boot -> bootstrap module. (0x 0400 0000)
2. bootstrap -> high mapped memory. (0x F400 0000)
3. paging table switch to: 
   0x6000 ~ 0x7000 (kernel), 0x7000 ~ 0x8000 (user)

4. SMP's AP boot code: 0x8000

----

x86 memory map:
```
1. 0x0000 0000 ~ 0x0000 04ff: BIOS data area.
2. 0x0000 0500 ~ 0x0007 f000: 510.75 kbytes, Free to use.
3. 0x0008 0000 ~ 0x0009 ffff: 128 kbytes, EBDA.
4. 0x000a 0000 ~ 0x000b ffff: 128 kbytes, Video Memory.
5. 0x000c 0000 ~ 0x000c 7fff: 32 kbytes, Video BIOS.
6. 0x000c 8000 ~ 0x000e ffff: 160 kbytes, BIOS expansions.
7. 0x000f 0000 ~ 0x000f ffff: 64 kbytes, Motherboard BIOS.
```

these are hardcoded in `mmap.c` file.

-- 1MB ~ ... must be init by grub provided memory map.
8. 0x0010 0000 ~ 0x00ef ffff: 14 mbytes, Free to use.


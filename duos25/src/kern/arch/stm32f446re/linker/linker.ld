ENTRY(Reset_Handler)
MEMORY
{
	FLASH(RX): ORIGIN = 0x08000000, LENGTH = 512K
	SRAM(RWX): ORIGIN = 0x20000000, LENGTH = 128K /* combined both SRAM1 and SRAM2 */
}
/* Sections placement in the memory */
SECTIONS
{
	/* Code goes to text in flash memory -- we cannot relocate*/
	/* location counter must inside the sections */
	.text :
	{	_stext = .;
		*(.isr_vector)
		*(.text) /* All input file */
		*(.rodata)
		. = ALIGN(4);
		_etext = .;
	}> FLASH AT> FLASH /* VMA and LMA memory location we can write >FLASH */	
	/* Load address is FLASH and VMA is SRAM */
	_la_data = LOADADDR(.data);
	.data :
	{
		_sdata = .;
		*(.data)
		. = ALIGN(4);
		_edata = .;
	}>SRAM AT> FLASH
	/* placed only on VMA that is SRAM */
	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .; 
	}>SRAM
}

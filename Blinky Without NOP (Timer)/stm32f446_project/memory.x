/* Memory layout for STM32F446RE */
MEMORY
{
  /* Flash memory begins at 0x08000000 and has a size of 512K */
  FLASH : ORIGIN = 0x08000000, LENGTH = 512K
  
  /* RAM begins at 0x20000000 and has a size of 128K */
  RAM : ORIGIN = 0x20000000, LENGTH = 128K
}


target remote :3333
monitor arm semihosting enable
monitor reset halt
load
break main
continue


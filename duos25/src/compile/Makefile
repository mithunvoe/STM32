# Makefile for DUOS: Last Updated on October 28, 2022
MCU=stm32f446re
CC=arm-none-eabi-gcc
MACH=cortex-m4
TARGET = target
FPU = -mfloat-abi=softfp -mfpu=fpv4-sp-d16
OBJECT = object

OS_PATH=../kern
OS_SYS_LIB = $(OS_PATH)/arch/$(MCU)/sys_lib
USERLAND = ../userland
DEV=$(OS_PATH)/dev
SYSTEM_CONFIG=$(OS_PATH)/sys_config
# System Device Prototype


INCLUDE=-I$(OS_PATH)/include \
-I$(OS_PATH)/arch/include \
-I$(OS_PATH)/arch/include/cm4 \
-I$(OS_PATH)/arch/$(MCU)/include \
-I$(OS_PATH)/include/kern \
-I$(DEV)/include \
-I$(USERLAND)/include \
-I$(SYSTEM_CONFIG) \
-I$(OS_PATH)/include/sotom



MAPDIR = mapfiles
# use -g3 for symbol and debug
CFLAGS=-c \
-mcpu=$(MACH) \
-mthumb $(FPU) \
-std=gnu11 \
-Wall \
-O0

LDFLAGS = -nostdlib \
-nostartfiles \
-nodefaultlibs \
-fno-exceptions  \
-mcpu=$(MACH) \
-mthumb $(FPU) \
-T $(OS_PATH)/arch/$(MCU)/linker/linker.ld \
-Wl,-Map=$(MAPDIR)/duos.map \
-Wl,-Map=build/final.map \
-O0

.PHONY: all clean load flash

all: kstring.o \
kmath.o \
cm4.o \
mcu_info.o \
kfloat.o \
sys_clock.o \
sys_usart.o \
sys_gpio.o \
sys_timer.o \
kmain.o \
kstdio.o \
stm32_startup.o \
sys_init.o \
unistd.o \
times.o \
kunistd.o \
ktimes.o \
syscall.o \
serial_lin.o \
UsartRingBuffer.o \
timer.o \
debug.o \
sys_spi.o \
sys_err.o \
sys_rtc.o \
duos \
final.elf



# final.elf


kstdio.o: $(OS_PATH)/lib/kstdio.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
kstring.o: $(OS_PATH)/lib/kstring.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

kmain.o: $(OS_PATH)/kmain/kmain.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
sys_init.o: $(OS_PATH)/lib/kern/sys_init.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

sys_clock.o: $(OS_SYS_LIB)/sys_clock.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

mcu_info.o: $(OS_SYS_LIB)/mcu_info.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

sys_gpio.o: $(OS_SYS_LIB)/sys_gpio.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
sys_usart.o: $(OS_SYS_LIB)/sys_usart.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
sys_timer.o: $(OS_SYS_LIB)/sys_timer.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

kmath.o: $(OS_PATH)/lib/kmath.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
stm32_startup.o: $(OS_SYS_LIB)/stm32_startup.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

cm4.o: $(OS_PATH)/arch/cm4/cm4.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

kfloat.o: $(OS_PATH)/lib/kfloat.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

unistd.o: $(USERLAND)/utils/unistd.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

kunistd.o: $(OS_PATH)/lib/kunistd.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

ktimes.o: $(OS_PATH)/lib/ktimes.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
times.o: $(USERLAND)/utils/times.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@
	
syscall.o: $(OS_PATH)/syscall/syscall.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

serial_lin.o: $(OS_PATH)/lib/kern/serial_lin.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

UsartRingBuffer.o: $(OS_PATH)/lib/UsartRingBuffer.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

debug.o:$(OS_PATH)/lib/sotom/debug.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

timer.o: $(OS_PATH)/lib/sotom/timer.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

sys_spi.o: $(OS_SYS_LIB)/sys_spi.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

sys_err.o: $(OS_SYS_LIB)/sys_err.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

sys_rtc.o: $(OS_SYS_LIB)/sys_rtc.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

test_can.o: $(USERLAND)/utils/test_can.c
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $(OBJECT)/$@

duos: $(OBJECT)/kmath.o \
	$(OBJECT)/kfloat.o  \
	$(OBJECT)/cm4.o \
	$(OBJECT)/mcu_info.o \
	$(OBJECT)/kstring.o \
	$(OBJECT)/kmain.o \
	$(OBJECT)/stm32_startup.o \
	$(OBJECT)/sys_clock.o \
	$(OBJECT)/sys_usart.o \
	$(OBJECT)/sys_gpio.o \
	$(OBJECT)/kstdio.o \
	$(OBJECT)/sys_init.o \
	$(OBJECT)/unistd.o \
	$(OBJECT)/times.o \
	$(OBJECT)/serial_lin.o \
	$(OBJECT)/UsartRingBuffer.o \
	$(OBJECT)/debug.o \
	$(OBJECT)/timer.o \
	$(OBJECT)/sys_err.o \
	$(OBJECT)/sys_rtc.o  
	

	$(CC) $(LDFLAGS) $^ -o $(TARGET)/$@

#	$(OBJECT)/m5stack.o \
$	$(OBJECT)/m5stack_error.o  

final.elf: $(OBJECT)/kmath.o \
	$(OBJECT)/kfloat.o  \
	$(OBJECT)/cm4.o \
	$(OBJECT)/mcu_info.o \
	$(OBJECT)/kstring.o \
	$(OBJECT)/kmain.o \
	$(OBJECT)/stm32_startup.o \
	$(OBJECT)/sys_clock.o \
	$(OBJECT)/sys_usart.o \
	$(OBJECT)/sys_gpio.o \
	$(OBJECT)/kstdio.o \
	$(OBJECT)/sys_init.o \
	$(OBJECT)/unistd.o \
	$(OBJECT)/times.o \
	$(OBJECT)/serial_lin.o \
	$(OBJECT)/UsartRingBuffer.o \
	$(OBJECT)/debug.o \
	$(OBJECT)/timer.o \
	$(OBJECT)/sys_err.o \
	$(OBJECT)/sys_rtc.o 
	
	
	$(CC) $(LDFLAGS) $^ -o build/$@
	
# $(OBJECT)/m5stack.o 
# $(OBJECT)/m5stack_error.o 

clean:
	rm -rf $(OBJECT)/*.o $(MAPDIR)/*.map $(TARGET)/* *.o *.elf build/*

# Flashing helpers
# Use OpenOCD with ST-LINK and STM32F4 target. Programs the ELF and resets the MCU.
# Ensure the image is up-to-date when flashing
flash: final.elf
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f4x.cfg -c "program build/final.elf verify reset exit"

# Alias
load: flash

# Produce raw binary from ELF (useful for st-flash)
bin: final.elf
	arm-none-eabi-objcopy -O binary build/final.elf build/final.bin

# Flash using stlink tools (requires st-flash). Base address for STM32F4 flash.
stlink: bin
	st-flash --reset write build/final.bin 0x08000000


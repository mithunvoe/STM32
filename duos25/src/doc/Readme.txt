Introduction: 
DUOS refers to Dhaka University Operating System, an experimental educational operating system designed primarily to carry out the OS course assignments for CSE students at the University of Dhaka to understand its technologies. Since July 2022, the DUOS has been under continuous development, and the initial credit goes to the 25th batch of CSE, DU. The CSE DU develops DUOS  on an ARM processor and, more remarkably, on a Cortex-M4 v7 architecture. DUOS currently supports 32-bit ARMv7-M processor-based MCU such as  STM32F4xx. However, the DUOS plans to expand its features to accept other ARM processors (such as ARMv8 and ARMv9 ARM 32 and 64-bit architectures). The Operating system must be tiny, provide a development venue for the controller designer and intelligent system developers, and help the affiliated technology enterprise deliver industry-grade control systems. 

Duos Directory Structure:
duos
└── src
    ├── compile
    │   ├── Makefile
    │   ├── mapfiles
    │   ├── object
    │   └── target
    ├── doc
    │   └── Readme.txt
    └── kern
        ├── arch
        │   ├── cm4
        │   │   └── cm4.c
        │   ├── include
        │   │   └── cm4
        │   │       └── cm4.h
        │   └── stm32f446re
        │       ├── dev
        │       │   ├── clock.c
        │       │   ├── gpio.c
        │       │   ├── timer.c
        │       │   └── usart.c
        │       ├── include
        │       │   ├── dev
        │       │   │   ├── clock.h
        │       │   │   ├── gpio.h
        │       │   │   ├── timer.h
        │       │   │   └── usart.h
        │       │   └── sys
        │       │       ├── stm32_peps.h
        │       │       └── stm32_startup.h
        │       ├── linker
        │       │   └── linker.ld
        │       └── sys
        │           └── stm32_startup.c
        ├── dev
        ├── include
        │   ├── kern
        │   │   ├── errmsg.h
        │   │   ├── errno.h
        │   │   ├── syscall_def.h
        │   │   ├── sys_init.h
        │   │   └── unistd.h
        │   ├── kfloat.h
        │   ├── kmain.h
        │   ├── kmath.h
        │   ├── kstdio.h
        │   ├── kstring.h
        │   └── syscall.h
        ├── kmain
        │   └── kmain.c
        ├── lib
        │   ├── kfloat.c
        │   ├── kmath.c
        │   ├── kstdio.c
        │   ├── kstring.c
        │   └── sys_init.c
        ├── proc
        ├── syscall
        │   └── syscalls.c
        ├── thread
        └── vfs

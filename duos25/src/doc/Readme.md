# 💡 DUOS: Dhaka University Operating System

DUOS is an **experimental, educational operating system** developed by students from the Department of Computer Science and Engineering (CSE) at the University of Dhaka. Its primary goal is to provide a hands-on environment for CSE students to complete OS course assignments and gain a deeper understanding of operating system technologies.

-----

## 🎯 Project Overview & Goals

DUOS is a **tiny operating system** designed for embedded and resource-constrained environments.

  * **Educational:** Serve as the core platform for the CSE DU OS course assignments.
  * **Target Architecture:** Initially developed for the **ARM Cortex-M4 v7 architecture** (32-bit).
  * **Current Support:** Primarily supports **STM32F4xx** series microcontrollers (MCUs).
  * **Future Vision:** Expand support to other ARM architectures, including **ARMv8** and **ARMv9** (32-bit and 64-bit), and act as a development venue for **intelligent system developers** and **industry-grade control systems**.

Development on DUOS began in **July 2022**, with initial contributions credited to the 25th batch of CSE, DU.

-----

## 📂 Directory Structure

The project follows a modular structure centered within the `src` directory:

```
duos
└── src
    ├── compile/            # Build artifacts (Makefile, mapfiles, objects, targets)
    ├── doc/                # Documentation (includes this Readme)
    └── kern/               # Core kernel source code
        ├── arch/           # Architecture-specific code (e.g., Cortex-M4 and STM32F446RE)
        │   ├── cm4/
        │   └── stm32f446re/
        │       ├── dev/        # MCU-specific device drivers (clock, GPIO, timer, USART)
        │       ├── include/    # Platform-specific headers (PEPs, startup)
        │       └── linker/     # Linker script (linker.ld)
        ├── include/        # Kernel-wide headers (kmain, syscalls, types, libraries)
        ├── kmain/          # Kernel entry point (kmain.c)
        ├── lib/            # Kernel libraries and utilities (kstring, kstdio, math)
        ├── proc/           # Process management components (TBD)
        ├── syscall/        # System call implementation and dispatch
        ├── thread/         # Threading components (TBD)
        └── vfs/            # Virtual File System components (TBD)
```

-----

## 🛠️ Building the OS

The build process is managed by the top-level **`Makefile`** located in `src/compile/`.

  * **`Makefile`:** Contains the top-level rules for compiling the kernel and flashing it onto the target MCU.
  * **Build Artifacts:** Compiled objects, map files, and the final target binary are placed within `src/compile/object`, `src/compile/mapfiles`, and `src/compile/target`, respectively.

-----

## 🚀 Reading Strategy & Boot Path

For developers seeking to understand DUOS, follow this structured reading strategy:

### 1\. The Boot Path (Startup & Kernel Entry)

This sequence covers the code execution from MCU power-on to the start of the kernel:

  * **`linker.ld`** (`kern/arch/stm32f446re/linker/`): Defines the **memory layout** and how code sections (`.text`, `.data`, `.bss`) are placed in Flash and RAM.
  * **`Reset_Handler`** (`stm32_startup.c`): The MCU's initial entry point. It handles essential steps like copying initialized data from Flash to RAM (`.data`), clearing uninitialized data (`.bss`), and then jumping to the main kernel function.
  * **`kmain`** (`kern/kmain/kmain.c`): The core **kernel entry point** after startup.
  * **`__sys_init`** (`kern/lib/sys_init.c`): Executes **early system initialization**, including setting up the clock, critical peripherals, and timers required for the OS to function.

### 2\. Core Kernel Interfaces

Review these essential header files to grasp the fundamental kernel structures and communication mechanisms:

  * **`kmain.h`**: Main kernel-related definitions.
  * **`types.h`**: Platform-wide data type definitions.
  * **`syscall.h`** & **`syscall_def.h`**: Defines the system call interface and unique identifiers for each syscall.

### 3\. System Calls

This component handles the secure transition from application code to kernel-mode execution:

  * **`syscalls.c`** (`kern/syscall/`): Contains the **SVC (Supervisor Call) handler dispatcher**, which routes a user's system call request to the appropriate kernel function based on the syscall ID.

### 4\. Drivers and Libraries

Once the core is understood, explore how the kernel interacts with hardware and provides utility functions:

  * **Drivers (`kern/arch/.../dev/`)**:
      * `clock.c`, `gpio.c`, `timer.c`, `usart.c`: Platform-specific drivers for essential peripherals.
      * Headers are found under `kern/arch/.../include/dev/`.
  * **Kernel Libraries (`kern/lib/`)**:
      * `kstring.c`: Kernel-safe string manipulation functions.
      * `kstdio.c`: Kernel-safe I/O functions.
      * *Note: Utility structures like `UsartRingBuffer.c` are also located here for common functionality.*

-----

## 🤝 Contribution

DUOS is a continuous effort by the CSE DU community. Students and developers are encouraged to contribute to its expansion and refinement.
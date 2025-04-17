# Basic Blinky Setup – STM32F446RE with Rust (WITHOUT HAL)

This project is a minimal embedded Rust setup for the STM32F446RE microcontroller. It blinks an LED connected to pin **PA5** using direct register access – no HAL(Hardware Abstraction Layer) used.

---

## 🚀 Features

- Blink LED on PA5 using direct register manipulation
- Bare-metal Rust (`no_std`, `no_main`)
- Panic handler with rapid LED blink
- Panic triggered after 5 blinks in debug mode (for testing)
- Fully automated setup and debug scripts
- OpenOCD + GDB integration

---

## 🧰 Prerequisites

Ensure you have:

- Rust installed via [`rustup`](https://rustup.rs/)
- Other dependencies like `openocd`, and `gdb-multiarch`

---

## ⚙️ Setup Instructions

1. **Download the setup script**  
   [`setup_stm32_project.sh`](https://github.com/mithunvoe/STM32/blob/main/Basic%20Blinky%20Setup/setup_stm32_project.sh)

2. **Make it executable**  
   ```bash
   chmod +x setup_stm32_project.sh
   ```

3. **Run the setup script**  
   ```bash
   ./setup_stm32_project.sh
   ```

   This creates a new Rust project under `stm32f446_project/` and configures it.

---

## ▶️ Running the Project

Once the setup is complete:

1. **Move into the project folder**
   ```bash
   cd stm32f446_project
   ```

2. **To build and flash (debug mode):**
   ```bash
   ./run_debug.sh
   ```

3. **To build and flash (release mode):**
   ```bash
   ./run_release.sh
   ```

4. **Manual GDB debugging:**

   Open 2 terminals:

   - Terminal 1:
     ```bash
     ./debug1.sh
     ```

   - Terminal 2:
     ```bash
     ./debug2.sh
     ```

   Inside GDB:
   ```gdb
   target extended-remote :3333
   load
   monitor reset halt
   continue
   ```

---

## ⚠️ Note

This project **does not use any Hardware Abstraction Layer (HAL)** – everything is done through direct memory access and raw peripheral manipulation.

---

## 📜 License

MIT License

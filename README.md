# STM32 Projects

This repository contains various STM32 embedded development projects. Each folder within this repo is a self-contained project targeting different microcontroller use cases using Rust.

---

## 📁 Project List

### 🔹 [Basic Blinky Setup](./Basic%20Blinky%20Setup/)
A minimal setup for STM32F446RE using Rust that blinks an LED (PA5).  
Includes full toolchain setup, memory layout, OpenOCD integration, and debug scripts.

- 🔧 `cargo`-based embedded Rust project
- 🛠️ Uses `openocd` and `gdb-multiarch` for flashing and debugging
- ⚙️ Sets up `.cargo/config.toml` and linker scripts
- 🧪 Debug-mode triggers a panic after 5 blinks for testing

---

## 🧰 Prerequisites

Before working on these projects, make sure you have:

- Rust toolchain installed via [`rustup`](https://rustup.rs/)
- Target support: `thumbv7em-none-eabihf`
- Additional tools:
  ```bash
  cargo install cargo-binutils
  rustup component add llvm-tools-preview
  sudo apt install openocd gdb-multiarch
  ```

---

## 🛠️ How to Add Your Own STM32 Project

1. Create a new folder with a clear name:
   ```
   mkdir "My New Project"
   ```

2. Put your Rust-based STM32 project inside it (you can reuse setup scripts).

3. Push your changes to this repo:
   ```bash
   git add .
   git commit -m "Add My New Project"
   git push
   ```

---

## 📚 Resources

- [Embedded Rust Book](https://docs.rust-embedded.org/book/)
- [STM32F4 Reference Manual](https://www.st.com/resource/en/reference_manual/dm00135183.pdf)
- [Awesome Embedded Rust](https://github.com/rust-embedded/awesome-embedded-rust)

---

## 📜 License

MIT License. See `LICENSE` file for details.

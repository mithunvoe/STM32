#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
BOLD='\033[1m'
UNDERLINE='\033[4m'
RESET_BOLD='\033[22m'

NC='\033[0m' 


curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

rustup target add thumbv7em-none-eabihf 
cargo install cargo-binutils
rustup component add llvm-tools-preview

sudo apt install openocd gdb-multiarch

cargo new --bin stm32f446_project
cd stm32f446_project

mkdir -p .cargo && cat > .cargo/config.toml << 'EOF'
[build]
target = "thumbv7em-none-eabihf"  # Cortex-M4F target

[target.thumbv7em-none-eabihf]
runner = "gdb-multiarch -q -x openocd.gdb"
rustflags = [
  "-C", "link-arg=-Tlink.x",
  "-C", "force-frame-pointers=yes",  # Helps with debugging
]
EOF


cat > memory.x << 'EOF'
/* Memory layout for STM32F446RE */
MEMORY
{
  /* Flash memory begins at 0x08000000 and has a size of 512K */
  FLASH : ORIGIN = 0x08000000, LENGTH = 512K
  
  /* RAM begins at 0x20000000 and has a size of 128K */
  RAM : ORIGIN = 0x20000000, LENGTH = 128K
}
EOF

cat >> Cargo.toml <<'EOF'
cortex-m = "0.7.7"
cortex-m-rt = "0.7.3"
panic-halt = "0.2.0"
volatile-register = "0.2.1"

[profile.release]
opt-level = 'z'     # Optimize for size
lto = true          # Link-time optimization
codegen-units = 1   # Better optimization
debug = true        # Symbols are good for debugging
panic = 'abort'     # Abort on panic

[[bin]]
name = "stm32f446_project"
path = "src/main.rs"
harness = false
EOF

cat > openocd.gdb <<'EOF'

target remote :3333
monitor arm semihosting enable
monitor reset halt
load
break main
continue

EOF

cat > src/main.rs <<'EOF'

#![no_std]
#![no_main]

use core::panic::PanicInfo;
use cortex_m_rt::entry;
use core::ptr::{read_volatile, write_volatile};

// Constants for STM32F446RE
const RCC_BASE: u32 = 0x40023800;
const RCC_AHB1ENR: u32 = RCC_BASE + 0x30;
const GPIOA_BASE: u32 = 0x40020000;
const GPIOA_MODER: u32 = GPIOA_BASE + 0x00;
const GPIOA_ODR: u32 = GPIOA_BASE + 0x14;
const PA5: u32 = 5;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    // Flash LED rapidly on panic
    unsafe {
        let rcc_ahb1enr = read_volatile(RCC_AHB1ENR as *const u32);
        write_volatile(RCC_AHB1ENR as *mut u32, rcc_ahb1enr | (1 << 0));
        
        let gpioa_moder = read_volatile(GPIOA_MODER as *const u32);
        write_volatile(GPIOA_MODER as *mut u32, gpioa_moder | (0b01 << (PA5 * 2)));
        
        loop {
            write_volatile(GPIOA_ODR as *mut u32, 1 << PA5);
            delay(100_00);
            write_volatile(GPIOA_ODR as *mut u32, 0);
            delay(100_00);
        }
    }
}

#[entry]
fn main() -> ! {
    // Enable GPIOA clock
    unsafe {
        write_volatile(RCC_AHB1ENR as *mut u32, 
                      read_volatile(RCC_AHB1ENR as *const u32) | (1 << 0));
    }

    // Configure PA5 as output
    unsafe {
        let moder = read_volatile(GPIOA_MODER as *const u32);
        write_volatile(GPIOA_MODER as *mut u32, 
                      (moder & !(0b11 << (PA5 * 2))) | (0b01 << (PA5 * 2)));
    }

    let mut counter = 0;
    loop {
        // Toggle LED
        unsafe {
            let odr = read_volatile(GPIOA_ODR as *const u32);
            write_volatile(GPIOA_ODR as *mut u32, odr ^ (1 << PA5));
        }

        // Debug-only fault after 5 blinks
        counter += 1;
        if cfg!(debug_assertions) && counter == 5 {
            panic!("Debug fault triggered");
        }

        delay(1_000_00); // Slower blink in debug
    }
}

#[inline(never)]
fn delay(cycles: u32) {
    for _ in 0..cycles {
        cortex_m::asm::nop();
    }
}

EOF

# cat > build.sh <<'EOF'
# #!/bin/bash
# cargo build
# cargo build --release
# # openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
# EOF

# cat > flash.sh <<'EOF'

# # sudo cargo run --release
# # gdb-multiarch target/thumbv7em-none-eabihf/debug/stm32f446_project

# #!/bin/bash
# openocd \
#   -f interface/stlink.cfg \
#   -f target/stm32f4x.cfg \
#   -c "program target/thumbv7em-none-eabihf/release/stm32f446_project verify reset exit"
# EOF

cat > run_debug.sh <<'EOF'
#!/bin/bash

#build
cargo build

#flash

openocd \
  -f interface/stlink.cfg \
  -f target/stm32f4x.cfg \
  -c "program target/thumbv7em-none-eabihf/debug/stm32f446_project verify reset exit"

EOF

cat > run_release.sh <<'EOF'
#!/bin/bash
#build
cargo build --release
#flash
openocd \
  -f interface/stlink.cfg \
  -f target/stm32f4x.cfg \
  -c "program target/thumbv7em-none-eabihf/release/stm32f446_project verify reset exit"
EOF

cat > debug1.sh <<'EOF'
#!/bin/bash
# Terminal 1: OpenOCD server
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg

EOF
cat > debug2.sh <<'EOF' 
# In separate terminal:
gdb-multiarch target/thumbv7em-none-eabihf/debug/stm32f446_project
EOF

# Then in GDB:
#   target extended-remote :3333
#   load
#   monitor reset halt
#   continue
# In another terminal, run:

chmod +x run_debug.sh run_release.sh debug1.sh debug2.sh

printf "\n${GREEN}Setup complete!${NC}\n"

# printf "${WHITE}Now move to the 'stm32f446_project' directory and run ${BOLD}${BLUE}'./build.sh'${RESET_BOLD}${WHITE} to build the STM32F446RE project and ${BOLD}${BLUE}'./flash.sh'${RESET_BOLD}${WHITE} to flash it.${NC}\n\n"
printf "${WHITE}Now move to the  ${BOLD}${PURPLE}stm32f446_project ${RESET_BOLD}${PURPLE} directory and run ${BOLD}${BLUE}'./run_debug.sh'${RESET_BOLD}${WHITE} to build and flash the debug version and ${BOLD}${BLUE}'./run_release.sh'${RESET_BOLD}${WHITE} to build and flash the release version.${NC}\n\n"

printf "${WHITE}You can also run ${BOLD}${BLUE}'./debug1.sh'${RESET_BOLD}${WHITE} in one terminal and ${BOLD}${BLUE}'./debug2.sh'${RESET_BOLD}${WHITE} in another terminal to start the OpenOCD server and GDB client.${NC}\n\n"

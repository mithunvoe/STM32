#![no_std]
#![no_main]

use cortex_m_rt::entry;
use cortex_m::asm;
mod clock; // bring clock.rs module
use core::panic::PanicInfo;

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {} // Just loop infinitely on panic
}

#[entry]
fn main() -> ! {
    clock::init_clock(); // Call our clock initialization

    loop {
        asm::nop();
    }
}

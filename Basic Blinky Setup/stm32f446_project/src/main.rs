
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
        if cfg!(debug_assertions) && counter == 10 {
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


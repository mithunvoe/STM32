#![no_std]
#![no_main]

use cortex_m_rt::entry;
use panic_halt as _;

mod registers;
mod rcc_config;
mod timer_config;

use registers::{RCC, GPIOA};
use rcc_config::configure_system_clock;
use timer_config::{configure_timer, delay_s};

#[entry]
fn main() -> ! {
    // Configure system clock
    configure_system_clock();
    
    // Configure GPIO
    unsafe {
        // Enable GPIOA clock
        RCC.as_mut().unwrap().ahb1enr.value |= (1 << 0);  // GPIOAEN
        
        // Configure PA5 as output
        GPIOA.as_mut().unwrap().moder.value &= !(0x3 << (5 * 2));  // Clear MODER5
        GPIOA.as_mut().unwrap().moder.value |= (0x1 << (5 * 2));   // MODER5 = 01 (Output)
    }
    
    // Configure timer
    configure_timer();
    
    loop {
        // Set PA5 high
        unsafe {
            GPIOA.as_mut().unwrap().bsrr.value = 1 << 5;
        }
        delay_s(3);        
        // Set PA5 low
        unsafe {
            GPIOA.as_mut().unwrap().bsrr.value = 1 << (5 + 16);
        }
        delay_s(3);
    }
}


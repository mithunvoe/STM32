#![no_std]
#![no_main]

use cortex_m_rt::entry;
use panic_halt as _;

mod rcc_config;
mod registers;
mod timer_config;

use rcc_config::configure_system_clock;
use registers::{GPIOA, RCC};
use stm32f4::stm32f446::{self, Peripherals};
use timer_config::{configure_timer, delay_s};

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    dp.GPIOA.moder.modify(|_, w| w.moder5().output());


    loop {
        dp.GPIOA.bsrr.write(|w| w.bs5().set_bit());
        delay_s(2);

        dp.GPIOA.bsrr.write(|w| w.br5().set_bit());
        delay_s(2);
    }
}

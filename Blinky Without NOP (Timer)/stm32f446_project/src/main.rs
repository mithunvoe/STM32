#![no_std]
#![no_main]

use cortex_m_rt::entry;
use panic_halt as _;

mod rcc_config;
mod timer_config;

use rcc_config::configure_system_clock;
use stm32f4::stm32f446::{self, Peripherals};
use timer_config::{configure_timer, delay_s};
const TESTING_FACTOR: u16 = 5;

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    gpio_init(&dp.GPIOA, 9, 0b01); // Red 1
    gpio_init(&dp.GPIOA, 8, 0b01); // Yellow 2
    gpio_init(&dp.GPIOA, 10, 0b01); // Red 2
    gpio_init(&dp.GPIOA, 6, 0b01); // Yellow 1
    gpio_init(&dp.GPIOA, 11, 0b01); // Green 1
    gpio_init(&dp.GPIOA, 12, 0b01); // Green 2
    gpio_init(&dp.GPIOA, 4, 0b00); // Left sensor
    gpio_init(&dp.GPIOA, 7, 0b00); // Right sensor

    dp.GPIOA.pupdr.modify(|_, w| w.pupdr4().pull_down());
    dp.GPIOA.pupdr.modify(|_, w| w.pupdr7().pull_down());

    loop {
        let left_e_genjam = dp.GPIOA.idr.read().idr4().bit();
        let right_e_genjam = dp.GPIOA.idr.read().idr7().bit();

        if left_e_genjam == right_e_genjam {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Set);

            delay_s(15 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Set);

            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Set);
            delay_s(15 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Set);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Reset);
        } else if left_e_genjam == true {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Set);
            delay_s(10 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Set);
            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Set);
            delay_s(30 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Set);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Reset);
        } else if right_e_genjam == true {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Set);
            delay_s(30 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Set);
            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Set);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Set);
            delay_s(10 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Set);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::Reset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::Reset);
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum GpioPinState {
    Reset = 0,
    Set = 1,
}

pub fn gpio_write_pin(gpio: &stm32f4::stm32f446::GPIOA, pin: u16, state: GpioPinState) {
    match state {
        GpioPinState::Set => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << pin) });
        }
        GpioPinState::Reset => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << (pin + 16)) });
        }
    }
}

pub fn gpio_init(gpio: &stm32f4::stm32f446::GPIOA, pin: u16, mode: u8) {
    gpio.moder.modify(|r, w| unsafe {
        let mut bits = r.bits();
        bits &= !(0b11 << (pin * 2));       // Clear the 2 bits for the pin
        bits |= (mode as u32) << (pin * 2); // Set the new mode
        w.bits(bits)
    });
}

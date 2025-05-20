#![no_std]
#![no_main]

use cortex_m_rt::entry;
use panic_halt as _;

mod rcc_config;
mod timer_config;

use rcc_config::configure_system_clock;
use stm32f4::stm32f446::{self, Peripherals};
use timer_config::{configure_timer, delay_s};
const TESTING_FACTOR: u16 = 1;

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    dp.GPIOA.moder.modify(|_, w| w.moder9().output());
    dp.GPIOA.moder.modify(|_, w| w.moder8().output());
    dp.GPIOA.moder.modify(|_, w| w.moder10().output());
    dp.GPIOA.moder.modify(|_, w| w.moder6().output());
    dp.GPIOA.moder.modify(|_, w| w.moder11().output());
    dp.GPIOA.moder.modify(|_, w| w.moder12().output());

    dp.GPIOA.moder.modify(|_, w| w.moder4().input());
    dp.GPIOA.moder.modify(|_, w| w.moder7().input());

    dp.GPIOA.pupdr.modify(|_, w| w.pupdr4().pull_down());
    dp.GPIOA.pupdr.modify(|_, w| w.pupdr7().pull_down());

    loop {
        let left_e_genjam = dp.GPIOA.idr.read().idr4().bit();
        let right_e_genjam = dp.GPIOA.idr.read().idr7().bit();

        if left_e_genjam == right_e_genjam {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinSet);

            delay_s(15 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinSet);

            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinSet);
            delay_s(15 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinSet);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinReset);
        } else if left_e_genjam == true {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinSet);
            delay_s(10 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinSet);
            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinSet);
            delay_s(30 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinSet);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinReset);
        } else if right_e_genjam == true {
            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinSet);
            delay_s(30 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 6, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinSet);
            delay_s(5 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 9, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 11, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinSet);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinSet);
            delay_s(10 / TESTING_FACTOR);

            gpio_write_pin(&dp.GPIOA, 10, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinSet);
            delay_s(5 / TESTING_FACTOR);
            gpio_write_pin(&dp.GPIOA, 8, GpioPinState::GpioPinReset);
            gpio_write_pin(&dp.GPIOA, 12, GpioPinState::GpioPinReset);
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum GpioPinState {
    GpioPinReset = 0,
    GpioPinSet = 1,
}

pub fn gpio_write_pin(gpio: &stm32f4::stm32f446::GPIOA, pin: u16, state: GpioPinState) {
    match state {
        GpioPinState::GpioPinSet => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << pin) });
        }
        GpioPinState::GpioPinReset => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << (pin + 16)) });
        }
    }
}

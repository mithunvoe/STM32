#![no_std]
#![no_main]

use core::sync::atomic::Ordering;
use cortex_m_rt::entry;
use panic_halt as _;

mod rcc_config;
mod timer_config;
mod constants;
mod gpio_helpers;
mod interreupt_helpers;
mod traffic;

use cortex_m::peripheral::Peripherals as CorePeripherals;
use rcc_config::configure_system_clock;
use stm32f4::stm32f446::{self, NVIC, Peripherals, interrupt};
use timer_config::{configure_timer, delay_s};

use constants::*;
use gpio_helpers::{gpio_init, gpio_write_pin};
use interreupt_helpers::configure_blink_timer;
use traffic::{LEFT_TRAFFIC_INTENSITY_LEVEL, RIGHT_TRAFFIC_INTENSITY_LEVEL, get_traffic_delays};

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let mut cp = CorePeripherals::take().unwrap();
    cp.DWT.enable_cycle_counter();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    dp.RCC.apb2enr.modify(|_, w| w.syscfgen().set_bit());

    dp.SYSCFG
        .exticr2
        .modify(|_, w| unsafe { w.exti4().bits(0b0000) });
    dp.EXTI.rtsr.modify(|_, w| w.tr4().set_bit());
    dp.EXTI.imr.modify(|_, w| w.mr4().set_bit());

    dp.SYSCFG
        .exticr2
        .modify(|_, w| unsafe { w.exti7().bits(0b0000) });
    dp.EXTI.rtsr.modify(|_, w| w.tr7().set_bit());
    dp.EXTI.imr.modify(|_, w| w.mr7().set_bit());

    configure_blink_timer(&dp);

    unsafe {
        NVIC::unmask(interrupt::EXTI4);
        NVIC::unmask(interrupt::EXTI9_5);
        NVIC::unmask(interrupt::TIM3);
    }

    gpio_init(&dp.GPIOA, RED_LEFT, 0b01);
    gpio_init(&dp.GPIOA, YELLOW_LEFT, 0b01);
    gpio_init(&dp.GPIOA, GREEN_LEFT, 0b01);
    gpio_init(&dp.GPIOA, GREEN_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, YELLOW_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, RED_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, LEFT_TRAFFIC_INTENSITY, 0b00);
    gpio_init(&dp.GPIOA, RIGHT_TRAFFIC_INSTENSITY, 0b00);
    gpio_init(&dp.GPIOA, 5, 0b01);
    gpio_init(&dp.GPIOA, LEFT_TRAFFIC_INDICATOR, 0b01);
    gpio_init(&dp.GPIOA, RIGHT_TRAFFIC_INDICATOR, 0b01);

    dp.GPIOA.pupdr.modify(|_, w| w.pupdr4().pull_down());
    dp.GPIOA.pupdr.modify(|_, w| w.pupdr7().pull_down());

    loop {
        let left_intensity = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
        let right_intensity = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);

        let mut delay = get_traffic_delays(left_intensity, right_intensity);

        gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);

        delay_s(delay[0] / TESTING_FACTOR);

        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);

        delay_s(delay[1] / TESTING_FACTOR);

        let left_intensity = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
        let right_intensity = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);

        delay = get_traffic_delays(left_intensity, right_intensity);

        gpio_write_pin(&dp.GPIOA, RED_LEFT, OFF);
        gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, OFF);
        gpio_write_pin(&dp.GPIOA, GREEN_LEFT, ON);
        gpio_write_pin(&dp.GPIOA, RED_RIGHT, ON);
        delay_s(delay[2] / TESTING_FACTOR);

        gpio_write_pin(&dp.GPIOA, GREEN_LEFT, OFF);
        gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, ON);
        delay_s(delay[3] / TESTING_FACTOR);
        gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, OFF);
        gpio_write_pin(&dp.GPIOA, RED_RIGHT, OFF);
    }
}
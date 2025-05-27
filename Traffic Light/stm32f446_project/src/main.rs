#![no_std]
#![no_main]

use core::sync::atomic::{AtomicBool, Ordering};
use cortex_m_rt::entry;
use panic_halt as _;

mod rcc_config;
mod timer_config;

use cortex_m::peripheral::DWT;
use cortex_m::peripheral::Peripherals as CorePeripherals;
use rcc_config::configure_system_clock;
use stm32f4::stm32f446::{self, EXTI, NVIC, Peripherals, interrupt};
use timer_config::{configure_timer, delay_s};

static mut LAST_EXTI4_TICK: u32 = 0;
static mut LAST_EXTI9_5_TICK: u32 = 0;

const DEBOUNCE_DELAY_MS: u32 = 20000;

// Static variables to store the traffic state
static LEFT_TRAFFIC_HIGH: AtomicBool = AtomicBool::new(false);
static RIGHT_TRAFFIC_HIGH: AtomicBool = AtomicBool::new(false);

const TESTING_FACTOR: u16 = 5;

const RED_LEFT: u16 = 9;
const YELLOW_LEFT: u16 = 8;
const GREEN_LEFT: u16 = 10;
const GREEN_RIGHT: u16 = 6;
const YELLOW_RIGHT: u16 = 11;
const RED_RIGHT: u16 = 12;
const LEFT_TRAFFIC_INTENSITY: u16 = 4;
const RIGHT_TRAFFIC_INSTENSITY: u16 = 7;

const ON: bool = true;
const OFF: bool = false;

// Define constants for the indicator pins
const LEFT_TRAFFIC_INDICATOR: u16 = 5;  // PA5
const RIGHT_TRAFFIC_INDICATOR: u16 = 15; // PA6

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let mut cp = CorePeripherals::take().unwrap();
    cp.DCB.enable_trace();
    cp.DWT.enable_cycle_counter();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    dp.RCC.apb2enr.modify(|_, w| w.syscfgen().set_bit()); // Enable SYSCFG clock for EXTI

    // Configure EXTI line for pin 4 (LEFT_TRAFFIC_INTENSITY)
    dp.SYSCFG
        .exticr2
        .modify(|_, w| unsafe { w.exti4().bits(0b0000) }); // PA4 source
    dp.EXTI.rtsr.modify(|_, w| w.tr4().set_bit()); // Rising trigger
    dp.EXTI.imr.modify(|_, w| w.mr4().set_bit()); // Unmask interrupt

    // Configure EXTI line for pin 7 (RIGHT_TRAFFIC_INSTENSITY)
    dp.SYSCFG
        .exticr2
        .modify(|_, w| unsafe { w.exti7().bits(0b0000) }); // PA7 source
    dp.EXTI.rtsr.modify(|_, w| w.tr7().set_bit()); // Rising trigger
    dp.EXTI.imr.modify(|_, w| w.mr7().set_bit()); // Unmask interrupt

    // Enable EXTI interrupts in NVIC
    unsafe {
        NVIC::unmask(interrupt::EXTI4);
        NVIC::unmask(interrupt::EXTI9_5);
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

    let delay_normal: [u16; 4] = [15, 5, 15, 5];
    let delay_left_intense: [u16; 4] = [10, 5, 30, 5];
    let delay_right_intense: [u16; 4] = [30, 5, 10, 5];
    // gpio_write_pin(&dp.GPIOA, 5, ON);
    loop {
        let left_traffic_high = LEFT_TRAFFIC_HIGH.load(Ordering::Relaxed);
        let right_traffic_high = RIGHT_TRAFFIC_HIGH.load(Ordering::Relaxed);

        let mut delay: [u16; 4] = [1, 1, 1, 1];

        if left_traffic_high == right_traffic_high {
            delay = delay_normal;
        } else if left_traffic_high {
            delay = delay_left_intense;
        } else if right_traffic_high {
            delay = delay_right_intense;
        }

        gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);

        delay_s(delay[0] / TESTING_FACTOR);

        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);

        delay_s(delay[1] / TESTING_FACTOR);

        let left_traffic_high = LEFT_TRAFFIC_HIGH.load(Ordering::Relaxed);
        let right_traffic_high = RIGHT_TRAFFIC_HIGH.load(Ordering::Relaxed);
        if left_traffic_high == right_traffic_high {
            delay = delay_normal;
        } else if left_traffic_high {
            delay = delay_left_intense;
        } else if right_traffic_high {
            delay = delay_right_intense;
        }

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


#[interrupt]
fn EXTI4() {
    unsafe {
        let now = DWT::cycle_count();
        if now.wrapping_sub(LAST_EXTI4_TICK) > 16_000_000 / 1000 * DEBOUNCE_DELAY_MS {
            LAST_EXTI4_TICK = now;
            let current = LEFT_TRAFFIC_HIGH.load(Ordering::Relaxed);
            let new_state = !current;
            LEFT_TRAFFIC_HIGH.store(new_state, Ordering::Relaxed);
            
            // Update LED indicator immediately
            let dp = stm32f446::Peripherals::steal();
            let gpioa = &dp.GPIOA;
            gpio_write_pin(gpioa, LEFT_TRAFFIC_INDICATOR, new_state);
        }
        (*EXTI::ptr()).pr.modify(|_, w| w.pr4().set_bit()); // clear interrupt
    }
}

#[interrupt]
fn EXTI9_5() {
    unsafe {
        let now = DWT::cycle_count();
        if now.wrapping_sub(LAST_EXTI9_5_TICK) > 16_000_000 / 1000 * DEBOUNCE_DELAY_MS {
            LAST_EXTI9_5_TICK = now;
            let current = RIGHT_TRAFFIC_HIGH.load(Ordering::Relaxed);
            let new_state = !current;
            RIGHT_TRAFFIC_HIGH.store(new_state, Ordering::Relaxed);
            
            // Update LED indicator immediately
            let dp = stm32f446::Peripherals::steal();
            let gpioa = &dp.GPIOA;
            gpio_write_pin(gpioa, RIGHT_TRAFFIC_INDICATOR, new_state);
        }
        (*EXTI::ptr()).pr.modify(|_, w| w.pr7().set_bit()); // clear interrupt
    }
}

pub fn gpio_write_pin(gpio: &stm32f4::stm32f446::GPIOA, pin: u16, state: bool) {
    match state {
        true => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << pin) });
        }
        false => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << (pin + 16)) });
        }
    }
}

pub fn gpio_init(gpio: &stm32f4::stm32f446::GPIOA, pin: u16, mode: u8) {
    gpio.moder.modify(|r, w| unsafe {
        let mut bits = r.bits();
        bits &= !(0b11 << (pin * 2)); // Clear the 2 bits for the pin
        bits |= (mode as u32) << (pin * 2); // Set the new mode
        w.bits(bits)
    });
}

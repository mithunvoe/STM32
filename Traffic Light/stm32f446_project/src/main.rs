#![no_std]
#![no_main]

use core::sync::atomic::{AtomicBool, AtomicU8, Ordering};
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

const DEBOUNCE_DELAY_MS: u32 = 500;

static LEFT_TRAFFIC_INTENSITY_LEVEL: AtomicU8 = AtomicU8::new(0);
static RIGHT_TRAFFIC_INTENSITY_LEVEL: AtomicU8 = AtomicU8::new(0);

static LEFT_INDICATOR_RATE: AtomicU8 = AtomicU8::new(0);
static RIGHT_INDICATOR_RATE: AtomicU8 = AtomicU8::new(0);

static LEFT_BLINK_STATE: AtomicBool = AtomicBool::new(false);
static RIGHT_BLINK_STATE: AtomicBool = AtomicBool::new(false);
static LEFT_BLINK_COUNTER: AtomicU8 = AtomicU8::new(0);
static RIGHT_BLINK_COUNTER: AtomicU8 = AtomicU8::new(0);

const BLINK_OFF: u8 = 0;
const BLINK_SLOW: u8 = 1;
const BLINK_MEDIUM: u8 = 2;
const BLINK_FAST: u8 = 3;

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

const LEFT_TRAFFIC_INDICATOR: u16 = 5;
const RIGHT_TRAFFIC_INDICATOR: u16 = 15;

const NORMAL: u8 = 0;
const INTENSE: u8 = 1;
const HIGH_INTENSE: u8 = 2;

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let mut cp = CorePeripherals::take().unwrap();
    cp.DCB.enable_trace();
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

    let delay_normal: [u16; 4] = [15, 5, 15, 5];
    let delay_left_intense: [u16; 4] = [10, 5, 30, 5];
    let delay_left_high_intense: [u16; 4] = [10, 5, 50, 5];
    let delay_right_intense: [u16; 4] = [30, 5, 10, 5];
    let delay_right_high_intense: [u16; 4] = [50, 5, 10, 5];

    loop {
        let left_intensity = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
        let right_intensity = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);

        let mut delay: [u16; 4] = delay_normal;

        if left_intensity == right_intensity {
            delay = delay_normal
        } else if left_intensity == NORMAL {
            if right_intensity == INTENSE {
                delay = delay_right_intense;
            } else if right_intensity == HIGH_INTENSE {
                delay = delay_right_high_intense;
            }
        } else if left_intensity == INTENSE {
            if right_intensity == NORMAL {
                delay = delay_left_intense;
            } else if right_intensity == HIGH_INTENSE {
                delay = delay_right_intense;
            }
        } else if left_intensity == HIGH_INTENSE {
            if right_intensity == NORMAL {
                delay = delay_left_high_intense;
            } else if right_intensity == INTENSE {
                delay = delay_left_intense;
            }
        }

        gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);

        delay_s(delay[0] / TESTING_FACTOR);

        gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);

        delay_s(delay[1] / TESTING_FACTOR);

        let left_intensity = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
        let right_intensity = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);

        if left_intensity == right_intensity {
            delay = delay_normal
        } else if left_intensity == NORMAL {
            if right_intensity == INTENSE {
                delay = delay_right_intense;
            } else if right_intensity == HIGH_INTENSE {
                delay = delay_right_high_intense;
            }
        } else if left_intensity == INTENSE {
            if right_intensity == NORMAL {
                delay = delay_left_intense;
            } else if right_intensity == HIGH_INTENSE {
                delay = delay_right_intense;
            }
        } else if left_intensity == HIGH_INTENSE {
            if right_intensity == NORMAL {
                delay = delay_left_high_intense;
            } else if right_intensity == INTENSE {
                delay = delay_left_intense;
            }
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

            let current_level = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
            let new_level = (current_level + 1) % 3;
            LEFT_TRAFFIC_INTENSITY_LEVEL.store(new_level, Ordering::Relaxed);

            match new_level {
                NORMAL => LEFT_INDICATOR_RATE.store(BLINK_OFF, Ordering::Relaxed),
                INTENSE => LEFT_INDICATOR_RATE.store(BLINK_MEDIUM, Ordering::Relaxed),
                HIGH_INTENSE => LEFT_INDICATOR_RATE.store(BLINK_FAST, Ordering::Relaxed),
                _ => LEFT_INDICATOR_RATE.store(BLINK_OFF, Ordering::Relaxed),
            }

            if new_level == NORMAL {
                let dp = stm32f446::Peripherals::steal();
                gpio_write_pin(&dp.GPIOA, LEFT_TRAFFIC_INDICATOR, OFF);
            }
        }
        (*EXTI::ptr()).pr.modify(|_, w| w.pr4().set_bit());
    }
}

#[interrupt]
fn EXTI9_5() {
    unsafe {
        let now = DWT::cycle_count();
        if now.wrapping_sub(LAST_EXTI9_5_TICK) > 16_000_000 / 1000 * DEBOUNCE_DELAY_MS {
            LAST_EXTI9_5_TICK = now;

            let current_level = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
            let new_level = (current_level + 1) % 3;
            RIGHT_TRAFFIC_INTENSITY_LEVEL.store(new_level, Ordering::Relaxed);

            match new_level {
                NORMAL => RIGHT_INDICATOR_RATE.store(BLINK_OFF, Ordering::Relaxed),
                INTENSE => RIGHT_INDICATOR_RATE.store(BLINK_MEDIUM, Ordering::Relaxed),
                HIGH_INTENSE => RIGHT_INDICATOR_RATE.store(BLINK_FAST, Ordering::Relaxed),
                _ => RIGHT_INDICATOR_RATE.store(BLINK_OFF, Ordering::Relaxed),
            }

            if new_level == NORMAL {
                let dp = stm32f446::Peripherals::steal();
                gpio_write_pin(&dp.GPIOA, RIGHT_TRAFFIC_INDICATOR, OFF);
            }
        }
        (*EXTI::ptr()).pr.modify(|_, w| w.pr7().set_bit());
    }
}

#[interrupt]
fn TIM3() {
    unsafe {
        let dp = stm32f446::Peripherals::steal();
        let gpioa = &dp.GPIOA;

        let left_rate = LEFT_INDICATOR_RATE.load(Ordering::Relaxed);
        let right_rate = RIGHT_INDICATOR_RATE.load(Ordering::Relaxed);

        if left_rate > BLINK_OFF {
            let left_counter = LEFT_BLINK_COUNTER.load(Ordering::Relaxed);
            let mut update_left = false;

            match left_rate {
                BLINK_FAST => {
                    if left_counter % 3 == 0 {
                        update_left = true;
                    }
                }
                BLINK_MEDIUM => {
                    if left_counter % 8 == 0 {
                        update_left = true;
                    }
                }
                BLINK_SLOW => {
                    if left_counter % 6 == 0 {
                        update_left = true;
                    }
                }
                _ => {}
            }

            if update_left {
                let current_left_state = LEFT_BLINK_STATE.load(Ordering::Relaxed);
                let new_left_state = !current_left_state;
                LEFT_BLINK_STATE.store(new_left_state, Ordering::Relaxed);
                gpio_write_pin(
                    gpioa,
                    LEFT_TRAFFIC_INDICATOR,
                    if new_left_state { ON } else { OFF },
                );
            }

            LEFT_BLINK_COUNTER.store((left_counter + 1) % 12, Ordering::Relaxed);
        }

        if right_rate > BLINK_OFF {
            let right_counter = RIGHT_BLINK_COUNTER.load(Ordering::Relaxed);
            let mut update_right = false;

            match right_rate {
                BLINK_FAST => {
                    if right_counter % 3 == 0 {
                        update_right = true;
                    }
                }
                BLINK_MEDIUM => {
                    if right_counter % 8 == 0 {
                        update_right = true;
                    }
                }
                BLINK_SLOW => {
                    if right_counter % 6 == 0 {
                        update_right = true;
                    }
                }
                _ => {}
            }

            if update_right {
                let current_right_state = RIGHT_BLINK_STATE.load(Ordering::Relaxed);
                let new_right_state = !current_right_state;
                RIGHT_BLINK_STATE.store(new_right_state, Ordering::Relaxed);
                gpio_write_pin(
                    gpioa,
                    RIGHT_TRAFFIC_INDICATOR,
                    if new_right_state { ON } else { OFF },
                );
            }

            RIGHT_BLINK_COUNTER.store((right_counter + 1) % 12, Ordering::Relaxed);
        }

        dp.TIM3.sr.modify(|_, w| w.uif().clear_bit());
    }
}

fn configure_blink_timer(dp: &Peripherals) {
    dp.RCC.apb1enr.modify(|_, w| w.tim3en().set_bit());

    dp.TIM3.psc.write(|w| w.psc().bits(15999));

    dp.TIM3.arr.write(|w| w.arr().bits(100));

    dp.TIM3.dier.write(|w| w.uie().set_bit());

    dp.TIM3.cr1.modify(|_, w| w.cen().set_bit());
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
        bits &= !(0b11 << (pin * 2));
        bits |= (mode as u32) << (pin * 2);
        w.bits(bits)
    });
}

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

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());

    gpio_init(&dp.GPIOA, RED_LEFT, 0b01);
    gpio_init(&dp.GPIOA, YELLOW_LEFT, 0b01);
    gpio_init(&dp.GPIOA, GREEN_LEFT, 0b01);
    gpio_init(&dp.GPIOA, GREEN_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, YELLOW_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, RED_RIGHT, 0b01);
    gpio_init(&dp.GPIOA, LEFT_TRAFFIC_INTENSITY, 0b00);
    gpio_init(&dp.GPIOA, RIGHT_TRAFFIC_INSTENSITY, 0b00);

    dp.GPIOA.pupdr.modify(|_, w| w.pupdr4().pull_down());
    dp.GPIOA.pupdr.modify(|_, w| w.pupdr7().pull_down());

    let delay_normal: [u16; 4] = [15, 5, 15, 5];
    let delay_left_intense: [u16; 4] = [10, 5, 30, 5];
    let delay_right_intense: [u16; 4] = [30, 5, 10, 5];

    loop {
        let left_traffic_high = dp.GPIOA.idr.read().idr4().bit();
        let right_traffic_high = dp.GPIOA.idr.read().idr7().bit();

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

        // if left_traffic_high == right_traffic_high {
        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);

        //     delay_s(15 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);

        //     delay_s(5 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, ON);
        //     delay_s(15 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, ON);
        //     delay_s(5 / TESTING_FACTOR);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, OFF);
        // } else if left_traffic_high == true {
        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);
        //     delay_s(10 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);
        //     delay_s(5 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, ON);
        //     delay_s(30 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, ON);
        //     delay_s(5 / TESTING_FACTOR);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, OFF);
        // } else if right_traffic_high == true {
        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, ON);
        //     delay_s(30 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, ON);
        //     delay_s(5 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, RED_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_RIGHT, OFF);
        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, ON);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, ON);
        //     delay_s(10 / TESTING_FACTOR);

        //     gpio_write_pin(&dp.GPIOA, GREEN_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, ON);
        //     delay_s(5 / TESTING_FACTOR);
        //     gpio_write_pin(&dp.GPIOA, YELLOW_LEFT, OFF);
        //     gpio_write_pin(&dp.GPIOA, RED_RIGHT, OFF);
        // }
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

use core::sync::atomic::Ordering;
use cortex_m::peripheral::DWT;
use stm32f4::stm32f446::{self, EXTI, interrupt};

use crate::{constants::*};
use crate::gpio_helpers::gpio_write_pin;
use crate::traffic::{
    LEFT_BLINK_COUNTER, LEFT_BLINK_STATE, LEFT_INDICATOR_RATE, LEFT_TRAFFIC_INTENSITY_LEVEL,
    RIGHT_BLINK_COUNTER, RIGHT_BLINK_STATE, RIGHT_INDICATOR_RATE, RIGHT_TRAFFIC_INTENSITY_LEVEL,
};
use crate::uart::{uart_add_to_buffer, uart_get_string, uart_line_available, uart_send_string};

static mut LAST_EXTI4_TICK: u32 = 0;
static mut LAST_EXTI9_5_TICK: u32 = 0;

#[interrupt]
fn USART2() {
    let dp = unsafe { stm32f446::Peripherals::steal() };
    
    // Check if RX interrupt flag is set
    if dp.USART2.sr.read().rxne().bit_is_set() {
        let received_byte = dp.USART2.dr.read().bits() as u8;
        uart_add_to_buffer(received_byte);
        
        // Process commands immediately when a complete line is available
        if uart_line_available() {
            if let Some(command) = uart_get_string() {
                match command.as_str() {
                    "Left Low" => {
                        LEFT_TRAFFIC_INTENSITY_LEVEL.store(0, Ordering::Relaxed);
                        RIGHT_TRAFFIC_INTENSITY_LEVEL.store(0, Ordering::Relaxed);
                        uart_send_string(&dp, "Traffic intensity set to 0\r\n");
                    }
                    "Left Medium" => {
                        LEFT_TRAFFIC_INTENSITY_LEVEL.store(1, Ordering::Relaxed);
                        uart_send_string(&dp, "Left traffic intensity set to 1\r\n");
                    }
                    "Left High" => {
                        LEFT_TRAFFIC_INTENSITY_LEVEL.store(2, Ordering::Relaxed);
                        uart_send_string(&dp, "Left traffic intensity set to 2\r\n");
                    }
                    "Right Low" => {
                        RIGHT_TRAFFIC_INTENSITY_LEVEL.store(0, Ordering::Relaxed);
                        uart_send_string(&dp, "Traffic intensity set to 0\r\n");
                    }
                    "Right Medium" => {
                        RIGHT_TRAFFIC_INTENSITY_LEVEL.store(1, Ordering::Relaxed);
                        uart_send_string(&dp, "Right traffic intensity set to 1\r\n");
                    }
                    "Right High" => {
                        RIGHT_TRAFFIC_INTENSITY_LEVEL.store(2, Ordering::Relaxed);
                        uart_send_string(&dp, "Right traffic intensity set to 2\r\n");
                    }
                    "help" => {
                        uart_send_string(&dp, "Available commands:\r\n");
                        uart_send_string(&dp, "Left Low - Set left traffic intensity to low\r\n");
                        uart_send_string(&dp, "Left Medium - Set left traffic intensity to medium\r\n");
                        uart_send_string(&dp, "Left High - Set left traffic intensity to high\r\n");
                        uart_send_string(&dp, "Right Low - Set right traffic intensity to low\r\n");
                        uart_send_string(&dp, "Right Medium - Set right traffic intensity to medium\r\n");
                        uart_send_string(&dp, "Right High - Set right traffic intensity to high\r\n");
                    }
                    "status" => {
                        let left_intensity = LEFT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
                        let right_intensity = RIGHT_TRAFFIC_INTENSITY_LEVEL.load(Ordering::Relaxed);
                        uart_send_string(&dp, "Current traffic intensities:\r\n");
                        match left_intensity {
                            0 => uart_send_string(&dp, "Left Intensity: 0\r\n"),
                            1 => uart_send_string(&dp, "Left Intensity: 1\r\n"),
                            2 => uart_send_string(&dp, "Left Intensity: 2\r\n"),
                            _ => uart_send_string(&dp, "Left Intensity: Unknown\r\n"),
                        }
                        match right_intensity {
                            0 => uart_send_string(&dp, "Right Intensity: 0\r\n"),
                            1 => uart_send_string(&dp, "Right Intensity: 1\r\n"),
                            2 => uart_send_string(&dp, "Right Intensity: 2\r\n"),
                            _ => uart_send_string(&dp, "Right Intensity: Unknown\r\n"),
                        }
                    }
                    _ => {
                        uart_send_string(&dp, "Unknown command: ");
                        uart_send_string(&dp, command.as_str());
                        uart_send_string(&dp, "\r\n");
                    }
                }
            }
        }
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

pub fn configure_blink_timer(dp: &stm32f446::Peripherals) {
    dp.RCC.apb1enr.modify(|_, w| w.tim3en().set_bit());
    dp.TIM3.psc.write(|w| w.psc().bits(15999));
    dp.TIM3.arr.write(|w| w.arr().bits(100));
    dp.TIM3.dier.write(|w| w.uie().set_bit());
    dp.TIM3.cr1.modify(|_, w| w.cen().set_bit());
}

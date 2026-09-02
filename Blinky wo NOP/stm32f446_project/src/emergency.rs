#![no_std]
#![no_main]

use cortex_m_rt::entry;
use panic_halt as _;
use cortex_m::interrupt::{free, Mutex};
use cortex_m::peripheral::NVIC;
use stm32f4::stm32f446::{self, Peripherals, Interrupt};
use core::cell::RefCell;

mod helpers;
mod rcc_config;
mod registers;
mod timer_config;

use helpers::*;
use rcc_config::configure_system_clock;
use registers::{GPIOA, RCC};
use timer_config::{configure_timer, delay_s, get_current_time_ms};

const TESTING_FACTOR: u16 = 1;

// Global flag for emergency interrupt
static EMERGENCY_FLAG: Mutex<RefCell<bool>> = Mutex::new(RefCell::new(false));

#[entry]
fn main() -> ! {
    configure_system_clock();
    configure_timer();

    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };

    // Enable GPIOA clock
    dp.RCC.ahb1enr.modify(|_, w| w.gpioaen().set_bit());
    
    // Enable SYSCFG clock for EXTI configuration
    dp.RCC.apb2enr.modify(|_, w| w.syscfgen().set_bit());

    // Configure output pins (LEDs)
    dp.GPIOA.moder.modify(|_, w| w.moder9().output());   // Red 1
    dp.GPIOA.moder.modify(|_, w| w.moder8().output());   // Yellow 2  
    dp.GPIOA.moder.modify(|_, w| w.moder10().output());  // Red 2
    dp.GPIOA.moder.modify(|_, w| w.moder6().output());   // Yellow 1
    dp.GPIOA.moder.modify(|_, w| w.moder11().output());  // Green 1
    dp.GPIOA.moder.modify(|_, w| w.moder12().output());  // Green 2

    // Configure input pins
    dp.GPIOA.moder.modify(|_, w| w.moder4().input());    // Left sensor
    dp.GPIOA.moder.modify(|_, w| w.moder7().input());    // Right sensor
    dp.GPIOA.moder.modify(|_, w| w.moder5().input());    // Emergency pin
    
    // Configure PA5 as emergency interrupt pin
    setup_emergency_interrupt(&dp);

    // Turn off all LEDs initially
    turn_off_all_leds(&dp);

    loop {
        // Check if emergency interrupt occurred
        let emergency_active = free(|cs| {
            let flag = EMERGENCY_FLAG.borrow(cs).borrow();
            *flag
        });

        if emergency_active {
            // Emergency mode - all red lights
            emergency_mode(&dp);
            
            // Wait for emergency to clear (10 seconds)
            smart_delay_s(10, &dp);
            
            // Clear emergency flag
            free(|cs| {
                EMERGENCY_FLAG.borrow(cs).replace(false);
            });
            
            continue; // Restart normal operation
        }

        // Read sensors (your original logic, but fixed)
        let left_e_genjam = dp.GPIOA.idr.read().idr4().bit();
        let right_e_genjam = dp.GPIOA.idr.read().idr7().bit();

        // Your traffic light logic with smart delays
        if left_e_genjam == right_e_genjam {
            // Both sensors same state
            traffic_sequence_1(&dp);
        } else if left_e_genjam {
            // Left sensor active
            traffic_sequence_2(&dp);
        } else if right_e_genjam {
            // Right sensor active  
            traffic_sequence_3(&dp);
        }
    }
}

fn setup_emergency_interrupt(dp: &Peripherals) {
    // Configure PA5 for external interrupt
    // SYSCFG_EXTICR2 controls EXTI5 (bits 4-7)
    dp.SYSCFG.exticr2.modify(|_, w| unsafe { w.exti5().bits(0) }); // PA5 -> EXTI5
    
    // Configure EXTI5 for rising edge trigger
    dp.EXTI.rtsr.modify(|_, w| w.tr5().set_bit());   // Rising edge
    dp.EXTI.ftsr.modify(|_, w| w.tr5().clear_bit()); // Not falling edge
    
    // Enable EXTI5 interrupt 
    dp.EXTI.imr.modify(|_, w| w.mr5().set_bit());
    
    // Enable EXTI9_5 interrupt in NVIC
    unsafe {
        NVIC::unmask(Interrupt::EXTI9_5);
    }
}

fn turn_off_all_leds(dp: &Peripherals) {
    // Turn off all LEDs using BSRR reset bits (BR)
    dp.GPIOA.bsrr.write(|w| w
        .br6().set_bit()   // Yellow 1
        .br8().set_bit()   // Yellow 2  
        .br9().set_bit()   // Red 1
        .br10().set_bit()  // Red 2
        .br11().set_bit()  // Green 1
        .br12().set_bit()  // Green 2
    );
}

fn emergency_mode(dp: &Peripherals) {
    // Turn off all LEDs first
    turn_off_all_leds(dp);
    
    // Turn on both red LEDs
    dp.GPIOA.bsrr.write(|w| w
        .bs9().set_bit()   // Red 1
        .bs10().set_bit()  // Red 2
    );
}

fn traffic_sequence_1(dp: &Peripherals) {
    // dandik green + bamdik red
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs11().set_bit().bs10().set_bit());
    smart_delay_s(20 / TESTING_FACTOR, dp);
    
    // dandik yellow + bamdik red
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs6().set_bit().bs10().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
    
    // dandik red + bamdik green
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs12().set_bit());
    smart_delay_s(15 / TESTING_FACTOR, dp);
    
    // dandik red + bamdik yellow
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs8().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
}

fn traffic_sequence_2(dp: &Peripherals) {
    // Left sensor priority - shorter red time for dandik
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs11().set_bit().bs10().set_bit());
    smart_delay_s(10 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs6().set_bit().bs10().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs12().set_bit());
    smart_delay_s(30 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs8().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
}

fn traffic_sequence_3(dp: &Peripherals) {
    // Right sensor priority - longer green time for dandik
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs11().set_bit().bs10().set_bit());
    smart_delay_s(30 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs6().set_bit().bs10().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs12().set_bit());
    smart_delay_s(10 / TESTING_FACTOR, dp);
    
    turn_off_all_leds(dp);
    dp.GPIOA.bsrr.write(|w| w.bs9().set_bit().bs8().set_bit());
    smart_delay_s(5 / TESTING_FACTOR, dp);
}

// TRUE immediate response delay using WFI (Wait For Interrupt)
fn smart_delay_s(seconds: u16, dp: &Peripherals) {
    let start_time = get_current_time_ms();
    let target_time = start_time + (seconds as u32 * 1000);
    
    while get_current_time_ms() < target_time {
        // Check emergency flag before each WFI
        let emergency_active = free(|cs| {
            let flag = EMERGENCY_FLAG.borrow(cs).borrow();
            *flag
        });
        
        if emergency_active {
            return; // Exit IMMEDIATELY
        }
        
        // Sleep until ANY interrupt occurs (including EXTI5)
        // This wakes up INSTANTLY when PA5 goes high
        cortex_m::asm::wfi();
    }
}

// Helper function for millisecond delays (you need to implement this)
fn delay_ms(ms: u32) {
    // Use your timer to create millisecond delay
    // This is a placeholder - implement based on your timer_config
    for _ in 0..(ms * 1000) {
        cortex_m::asm::nop();
    }
}

// ULTRA-FAST: External interrupt handler that changes LEDs immediately
#[stm32f4::stm32f446::interrupt]
fn EXTI9_5() {
    // Get peripherals (this is safe in interrupt context)
    let dp = unsafe { stm32f446::Peripherals::steal() };
    
    // IMMEDIATELY turn on emergency lights (all red)
    // This happens in MICROSECONDS!
    dp.GPIOA.bsrr.write(|w| w
        .br6().set_bit()   // Turn off Yellow 1
        .br8().set_bit()   // Turn off Yellow 2
        .br11().set_bit()  // Turn off Green 1
        .br12().set_bit()  // Turn off Green 2
        .bs9().set_bit()   // Turn on Red 1
        .bs10().set_bit()  // Turn on Red 2
    );
    
    // Set emergency flag for main loop
    free(|cs| {
        EMERGENCY_FLAG.borrow(cs).replace(true);
    });
    
    // Clear the interrupt pending bit
    dp.EXTI.pr.write(|w| w.pr5().set_bit());
}
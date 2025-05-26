use crate::registers::{RCC, TIM6};

pub fn configure_timer() {
    unsafe {
        // Enable Timer clock
        RCC.as_mut().unwrap().apb1enr.value |= (1 << 4);  // TIM6EN

        // Configure timer
        
        TIM6.as_mut().unwrap().psc.value = 90 - 1;  // 90MHz/90 = 1MHz
        TIM6.as_mut().unwrap().arr.value = 0xFFFF;  // Max ARR value
        
        // Enable counter and wait for update flag
        TIM6.as_mut().unwrap().cr1.value |= (1 << 0);  // CEN
        while (TIM6.as_ref().unwrap().sr.value & (1 << 0)) == 0 {}  // UIF
    }
}

pub fn delay_us(us: u16) {
    unsafe {
        // Reset counter
        TIM6.as_mut().unwrap().cnt.value = 0;
        
        // Wait for counter to reach desired value
        while TIM6.as_ref().unwrap().cnt.value < us as u32 {}
    }
}

pub fn delay_ms(ms: u16) {
    for _ in 0..ms {
        delay_us(1000);
    }
}

pub fn delay_s(s: u16) {
    for _ in 0..s {
        delay_ms(1000);
    }
}
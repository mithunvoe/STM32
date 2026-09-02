use core::cell::RefCell;
use cortex_m::interrupt::Mutex;
use stm32f4::stm32f446::{self, TIM6};

static G_TIM6: Mutex<RefCell<Option<TIM6>>> = Mutex::new(RefCell::new(None));

pub fn configure_timer() {
    let dp = unsafe { stm32f446::Peripherals::steal() };

    dp.RCC.apb1enr.modify(|_, w| w.tim6en().set_bit());

    dp.TIM6.psc.write(|w| w.psc().bits(90 - 1));
    dp.TIM6.arr.write(|w| w.arr().bits(0xFFFF));

    dp.TIM6.cr1.modify(|_, w| w.cen().set_bit());

    while dp.TIM6.sr.read().uif().bit_is_clear() {}

    cortex_m::interrupt::free(|cs| {
        G_TIM6.borrow(cs).replace(Some(dp.TIM6));
    });
}

pub fn delay_us(us: u16) {
    cortex_m::interrupt::free(|cs| {
        if let Some(tim6) = G_TIM6.borrow(cs).borrow().as_ref() {
            tim6.cnt.reset();

            while (tim6.cnt.read().cnt().bits() as u16) < us {
                
            }
        }
    });
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

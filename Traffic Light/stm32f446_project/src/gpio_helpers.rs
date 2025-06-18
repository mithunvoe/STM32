use stm32f4::stm32f446::GPIOA;

pub fn gpio_write_pin(gpio: &GPIOA, pin: u16, state: bool) {
    match state {
        true => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << pin) });
        }
        false => {
            gpio.bsrr.write(|w| unsafe { w.bits(1 << (pin + 16)) });
        }
    }
}

pub fn gpio_init(gpio: &GPIOA, pin: u16, mode: u8) {
    gpio.moder.modify(|r, w| unsafe {
        let mut bits = r.bits();
        bits &= !(0b11 << (pin * 2));
        bits |= (mode as u32) << (pin * 2);
        w.bits(bits)
    });
}
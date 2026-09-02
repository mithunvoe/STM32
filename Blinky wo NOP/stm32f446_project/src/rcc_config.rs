use stm32f4::stm32f446::{self, Peripherals};

pub fn configure_system_clock() {
    let dp: Peripherals = unsafe { stm32f446::Peripherals::steal() };
    let rcc = &dp.RCC;

    rcc.cr.modify(|_, w| w.hseon().on());

    while rcc.cr.read().hserdy().is_not_ready() {}

    rcc.apb1enr.modify(|_, w| w.pwren().enabled());

    let pwr = &dp.PWR;
    pwr.cr.modify(|_, w| w.vos().variant(0b11));

    let flash = &dp.FLASH;
    flash.acr.modify(|_, w| {
        w.icen().enabled();
        w.dcen().enabled();
        w.prften().enabled();
        w.latency().ws5()
    });

    rcc.cfgr.modify(|_r, w| {
        w.hpre().div1();
        w.ppre1().div4();
        w.ppre2().div2()
    });

    rcc.pllcfgr.modify(|_, w| unsafe {
        w.pllm().bits(4);
        w.plln().bits(180);
        w.pllp().div2();
        w.pllsrc().hse()
    });

    rcc.cr.modify(|_, w| w.pllon().on());
    while rcc.cr.read().pllrdy().is_not_ready() {}

    rcc.cfgr.modify(|_, w| w.sw().pll());

    while !rcc.cfgr.read().sws().is_pll() {}
}

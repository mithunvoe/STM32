#![allow(dead_code)]

use stm32f4::stm32f446;

/// Initialize system clock to 180 MHz using HSE and PLL.
pub fn init_clock() {
    let dp = unsafe { stm32f446::Peripherals::steal() }; // SAFETY: We are early in startup, only one caller

    let rcc = &dp.RCC;
    let pwr = &dp.PWR;
    let flash = &dp.FLASH;

    // 1. Enable HSE and wait for ready
    rcc.cr.modify(|_, w| w.hseon().on());
    while rcc.cr.read().hserdy().is_not_ready() {}

    // 2. Power enable clock and voltage regulator
    rcc.apb1enr.modify(|_, w| w.pwren().enabled());
    //pwr.cr.modify(|_, w| w.vos().scale1());
    pwr.cr.modify(|_, w| w.vos().variant(0b11)); // Scale 1 is usually 0b11 or 0b01

    // 3. Configure flash
    flash.acr.modify(|_, w| {
        w.icen().enabled();
        w.dcen().enabled();
        w.prften().enabled();
        w.latency().ws5()
    });

    // 4. Prescalers
    rcc.cfgr.modify(|_, w| {
        w.hpre().div1();   // AHB = SYSCLK
        w.ppre1().div4();  // APB1 = SYSCLK/4
        w.ppre2().div2()   // APB2 = SYSCLK/2
    });

    // 5. Configure PLL (M=4, N=180, P=2)
    rcc.pllcfgr.write(|w| unsafe {
        w.pllm().bits(4)
         .plln().bits(180)
         .pllp().div2()
         .pllsrc().hse()
    });

    // 6. Enable PLL and wait for ready
    rcc.cr.modify(|_, w| w.pllon().on());
    while rcc.cr.read().pllrdy().is_not_ready() {}

    // 7. Switch system clock source to PLL
    rcc.cfgr.modify(|_, w| w.sw().pll());
    while !rcc.cfgr.read().sws().is_pll() {}
}
